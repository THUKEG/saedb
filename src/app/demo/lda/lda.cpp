#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include "sae_include.hpp"
using  namespace sae::io;
#define sz(x) x.size()



float RESET_PROB = 0.15;
float TOLERANCE = 1.0E-2;


typedef saedb::empty message_date_type;
/* ===============up is the sae engine============================*/


typedef int count_type;// Global Types

//typedef std::std::vector< sae::atomic<count_type> > factor_type;// 需要原子操作么？
typedef std::vector<count_type> factor_type;


inline factor_type& operator+=(factor_type& lvalue, const factor_type& rvalue) 
{
  if(!rvalue.empty()) 
  {
    if(lvalue.empty()) 
    	lvalue = rvalue;
    else 
    {
      for(size_t t = 0; t < lvalue.size(); ++t) lvalue[t] += rvalue[t];
    }
  }
  return lvalue;
} // end of operator +=

typedef int topic_id_type;

#define NULL_TOPIC (topic_id_type(-1))

typedef std::vector< topic_id_type > assignment_type;


// ============================================================================
// Global Variables

double ALPHA = 1;
double BETA = 0.1;
size_t NTOPICS = 50;
size_t NWORDS = 0;
size_t NDOCS = 0;
size_t NTOKENS = 0;
size_t TOPK = 5;
size_t INTERVAl = 10;

factor_type GLOBAL_TOPIC_COUNT;
std::vector<std::string> DICTIONARY;
size_t MAX_COUNT = 10000;
float BURNIN = -1;  // less than 0 then sampler will run indefinitely.

/* ============= 下面是graph的信息======================*/
struct vertex_data
{
	int nupdates;
	int nchanges;
	factor_type factor;
	vertex_data() : nupdates(0), nchanges(0), factor(NTOPICS) {};
}; //end of vertex_data

struct edge_data
{
	int nchanges;
	assignment_type assignment;
	edge_data(size_t ntokens =0 ) : nchanges(0), assignment(ntokens, NULL_TOPIC){};
}; // end of edge_data;
typedef saedb::empty message_date_type;
typedef saedb::sae_graph<vertex_data, edge_data> graph_type;

inline int is_word( graph_type::vertex_type&  vertex)//这个地方没有引用
{
	return vertex.num_in_edges() > 0 ? 1:0;
}

inline int is_doc( graph_type::vertex_type& vertex)//这个地方没有引用
{
	return vertex.num_out_edges() > 0 ? 1:0;
}

inline size_t count_tokens(graph_type::edge_type& edge)
{
	return edge.data().assignment.size();
}

inline graph_type::vertex_type get_other_vertex(graph_type::edge_type &edge, const graph_type::vertex_type& vertex)
{
	return vertex.id()==edge.source().id()?edge.target():edge.source();
}

/* ============= the collapsed gibbs Fsampler======================*/

struct gather_type
{
	factor_type factor;
	int nchanges;
	gather_type() : nchanges(0) {};
	gather_type(int nchanges) : factor(NTOPICS) , nchanges(nchanges){};
	gather_type& operator+=(const gather_type& other)
	{
		factor+=other.factor;
		nchanges+=other.nchanges;
		return *this;
	}
};




class cgs_lda_vertex_program:
public saedb::IAlgorithm<graph_type, gather_type>
{
public:
	static bool DISABLE_SAMPLING;
	
	//void init(icontext_type& context, vertex_type& vertex)
	//{
      
    //}//这步初始化可能不对
    
	edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const
	{
		return saedb::ALL_EDGES;
	}	
	
	gather_type gather(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const 
	{
		gather_type ret(edge.data().nchanges);
		const assignment_type& assignment = edge.data().assignment;
		return ret;
	}
// end of gather

	
	void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total)
    {
		const size_t num_neighbors = vertex.num_in_edges() + vertex.num_out_edges();
	
		vertex_data & vdata = vertex.data();
		vdata.nupdates++;
		vdata.nchanges = total.nchanges;
		vdata.factor = total.factor;
	}// end of apply
	
	edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
          if(DISABLE_SAMPLING || (BURNIN>0 ||  BURNIN<100 )) 
		  		return saedb::NO_EDGES;
		  else return saedb::ALL_EDGES;
    }//    context.elapsed_seconds();这个值没有

	
	 void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const
	{
		
		srandom(time(0));
		vertex_type vs=	edge.source();
		
		factor_type& doc_topic_count = is_word(vs) ? edge.source().data().factor : edge.target().data().factor;
		
		factor_type& word_topic_count = is_doc(vs) ? edge.source().data().factor : edge.target().data().factor;

		std::vector<double> prob(NTOPICS);
		assignment_type& assignment = edge.data().assignment;
		edge.data().nchanges =0 ;
		
		for(int i=0; i<sz(assignment); i++)
		{
			topic_id_type& asg = assignment[i];
			const topic_id_type old_asg=asg;
			if(asg != NULL_TOPIC)
			{
				--doc_topic_count[asg];
				--word_topic_count[asg];
				--GLOBAL_TOPIC_COUNT[asg];
			}
			for(size_t t=0 ; t<NTOPICS; t++)
			{
				double n_dt = max(count_type(doc_topic_count[asg]),count_type(0));
				double n_wt = max(count_type(word_topic_count[asg]),count_type(0));
				double n_t = max(count_type(GLOBAL_TOPIC_COUNT[asg]),count_type(0));
				prob[t]=(ALPHA + n_dt)*(BETA + n_wt)/(BETA*NWORDS + n_t);//总觉的这个地方的函数是少除以了一个(k*alpha+n_dt)
			//prob[t]=(ALPHA + n_dt)*(BETA + n_wt)/(BETA*NWORDS + n_t)*(ALPHA*NTOPICS+一个什么（普通的lda里面有）);
				
			}
			/*做multinomial抽样*/
			for(size_t t=1; t<NTOPICS-1; t++)			
				prob[t]+=prob[t-1];
			
			double u = ((double)random()/RAND_MAX)*prob[NTOPICS-1];
			size_t now=0;
			for(size_t now=0; now<NTOPICS-1; now++)
			if(prob[now]>u)
				break;
			
			asg =now;
			/*根据概率随机抽样*/
			
			--doc_topic_count[asg];
			--word_topic_count[asg];
			--GLOBAL_TOPIC_COUNT[asg];
			if(asg != old_asg)
			{
				++edge.data().nchanges;
			}
		}
		
		context.signal(get_other_vertex(edge, vertex));
	}
};


bool cgs_lda_vertex_program::DISABLE_SAMPLING = false;

typedef cgs_lda_vertex_program:: icontext_type icontext_type;

class topk_aggregator
{
	typedef pair<float, saedb::vertex_id_type> cw_pair_type;
	private:
		std::vector< std::set<cw_pair_type> >top_words;
		size_t nchanges, nupdates;
	public:
		topk_aggregator(size_t nchanges = 0, size_t nupdates =0 ): nchanges(nchanges), nupdates(nupdates){}
	
	topk_aggregator& operator+=(const topk_aggregator& other)
	{	
		nchanges += other.nchanges;
		nupdates += other.nupdates;
		if(other.top_words.empty()) return *this;
		if(top_words.empty())	top_words.resize(NTOPICS);
		for(size_t i=0; i<sz(top_words); i++)
		{
			top_words[i].insert(other.top_words[i].begin(),other.top_words[i].end());
			while( sz(top_words[i]) > TOPK)
				top_words[i].erase(top_words[i].begin());
		}
		return *this;
	} 
	
	static void finalize(icontext_type& context, const topk_aggregator& total)
	{
		
		std::cout<<1<<endl;
		//这部分应该是输出，暂且不弄
	}
};

struct global_counts_aggregator
{
	typedef graph_type::vertex_type vertex_type;
	static factor_type map(icontext_type& context, const vertex_type& vertex)
	{
		return vertex.data().factor;
	}
	
	static void finalize(icontext_type& context, const factor_type& total)
	{
		size_t sum=0 ;
		for(size_t t=0; t<sz(total); t++)
		{
			GLOBAL_TOPIC_COUNT[t] = max(count_type(total[t]/2), count_type(0));
			sum+= GLOBAL_TOPIC_COUNT[t];
		}
		//cout<<"total tokens"<<sum<<end;// 少了一句输出
	}
};

class likelihood_aggregator
{
	typedef graph_type::vertex_type vertex_type;
	double lik_words_given_topics;
	double lik_topics;
public:
	likelihood_aggregator() : lik_words_given_topics(0), lik_topics(){}
	
	likelihood_aggregator& operator += (const likelihood_aggregator& other)
	{
		lik_words_given_topics += other.lik_words_given_topics;
		lik_topics += other.lik_topics;
		return *this;
	}
	static likelihood_aggregator map(icontext_type& context, vertex_type& vertex)
	{
		const factor_type& factor = vertex.data().factor;
		likelihood_aggregator ret;
		if(is_word(vertex))
		{
			for(size_t t=0; t<NTOPICS; t++)
			{
				const double value = max(count_type(factor[t]),count_type(0));
				ret.lik_words_given_topics += lgamma(value + BETA);
			}
		}
		return ret;
	}
	
	static void finalize(icontext_type& context, const likelihood_aggregator& total)
	{
		double denominator = 0;
		for(size_t t = 0; t<NTOPICS; t++)
		{
			denominator += lgamma(GLOBAL_TOPIC_COUNT[t] + NWORDS*BETA);
		}
		
		const double lik_words_given_topics=
			NTOPICS*(lgamma(NWORDS * BETA) - NWORDS * lgamma(BETA))- denominator + total.lik_words_given_topics;
			
		const double lik_topics = NDOCS *(lgamma(NTOPICS * ALPHA) - NTOPICS * lgamma(ALPHA)) + total.lik_topics;
		const double lik = lik_words_given_topics + lik_topics;
		//输出lik就行
	}
}; // end of likelihood_aggregator struct

struct signal_only
{
	static saedb::empty docs(icontext_type& context, graph_type::vertex_type& vertex)
	{
		if(is_doc(vertex))	context.signal(vertex);
			return saedb::empty();
	}
	static saedb::empty words(icontext_type& context,  graph_type:: vertex_type& vertex)
	{
		if(is_word(vertex)) context.signal(vertex);
			return saedb::empty();
	}
};// end of signal



/*=========下面这是读入普通文件然后传入==================*/

void init()
{
	
}

int main()
{
	graph_type graph;
	
	GLOBAL_TOPIC_COUNT.resize(NTOPICS);
	
	saedb::IEngine<cgs_lda_vertex_program> *engine = new saedb::EngineDelegate<cgs_lda_vertex_program>(graph);
	
	/*===============初始化===============*/
	NWORDS =  engine ->map_reduce_vertices<size_t>(is_word);
	NDOCS =  engine ->map_reduce_vertices<size_t>(is_doc);
	//NTOKENS =  engine ->map_reduce_edges<size_t>(count_tokens);  edge的操作还没有
	
}
