#include <cmath>
#include <fstream>
#include <sstream>
#include "computing/computing.hpp"
#include "lda_instance.hpp"
#include "env.hpp"
env env_inst;//manage toolkit parameters

void sort_pair(vector<pair<int,double>>& vec);//used to sort
vector<string> line_tokenize(string a, string seps);//used to tokenize  
vector<string> component_tokenize(string a, string seps);//used to tokenize
typedef saedb::empty message_data_type;
typedef saedb::sae_graph graph_type;

bool is_doc(const graph_type::vertex_type& vertex)//judge whether it is word and doc
{
	return vertex.num_out_edges() > 0 ? true:false;
}

int id2k(int id)//map (id in env_inst.docList) to (key in SAE graph) for document node
{
	return -id-2;
}
int k2id(int key) //map (key in SAE graph) to (id in env_inst.docList) for document node
{
	return -key-2;
}



class lda_init:
public saedb::IAlgorithm<graph_type, double, message_data_type>
{
public:
	
	void init(icontext_type& context, vertex_type& vertex, const message_type& msg) {
	
    	}
	edge_dir_type gather_edges(icontext_type& context,const vertex_type& vertex) const{
		
		return saedb::IN_EDGES;
	}	
	
	double gather(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const 
	{
		edge_data e = edge.parse<edge_data>();
		int topic = rand()%env_inst.NTOPICS; //sample topic from Mean(1/K)
		e.assignment=topic;
		edge.update<edge_data>(e);
		vertex_data vs=edge.source().parse<vertex_data>();
		vertex_data vt=edge.target().parse<vertex_data>();
		vs.n[topic]++;
		vt.n[topic]++;
		env_inst.nwsum[topic]++;
		env_inst.ndsum[k2id(vs.id)]++;
		return 0;
	}

	
	void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total)
    	{
	}
	
	edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
		return saedb::NO_EDGES;
	}

	
	 void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const
	{
	}
};
class lda_gibbs:
public saedb::IAlgorithm<graph_type, double, message_data_type>
{
public:
	
	void init(icontext_type& context, vertex_type& vertex, const message_type& msg) {
		vertex_data v=vertex.parse<vertex_data>();
	
    	}
	edge_dir_type gather_edges(icontext_type& context,const vertex_type& vertex) const{
		
		return saedb::IN_EDGES;
	}	
	
	double gather(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const 
	{
		// remove topic from the count variables
	    	
		edge_data e=edge.parse<edge_data>();
		int topic = e.assignment;
		vertex_data vd=edge.source().parse<vertex_data>();
		vertex_data vw=edge.target().parse<vertex_data>();
		vw.n[topic]--;
		vd.n[topic]--;
		env_inst.nwsum[topic]--;
		env_inst.ndsum[k2id(vd.id)]--;
		
		// do multinomial sampling via cumulative method:
		double *p=new double[env_inst.NTOPICS];
		for (int k = 0; k < env_inst.NTOPICS; k++) {
		    p[k] = (vw.n[k] + env_inst.BETA) / (env_inst.nwsum[k] + env_inst.NWORDS * env_inst.BETA)
		        * (vd.n[k] + env_inst.ALPHA) / (env_inst.ndsum[k2id(vd.id)] + env_inst.NTOPICS * env_inst.ALPHA);
		}
		
		// cumulate multinomial parameters
		for (int k = 1; k < env_inst.NTOPICS; k++) {
		    p[k] += p[k - 1];
		}
		// scaled sample because of unnormalised p[]
		double u = (double)rand()/RAND_MAX * p[env_inst.NTOPICS - 1];
		for (topic = 0; topic < env_inst.NTOPICS; topic++) {
		    if (u <= p[topic])
		        break;
		}
		delete []p;

		// add newly estimated topic to count variables
		vw.n[topic]++;
		vd.n[topic]++;
		env_inst.nwsum[topic]++;
		env_inst.ndsum[k2id(vd.id)]++;
		e.assignment=topic;
		edge.update<edge_data>(e);
	}

	
	void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total)
    	{
	}
	
	edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
		return saedb::NO_EDGES;
	}

	
	 void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const
	{
	}
};
class lda_update:
public saedb::IAlgorithm<graph_type, double, message_data_type>
{
public:
	
	void init(icontext_type& context, vertex_type& vertex, const message_type& msg) {
		vertex_data v=vertex.parse<vertex_data>();
	
    	}
	edge_dir_type gather_edges(icontext_type& context,const vertex_type& vertex) const{
		
		return saedb::IN_EDGES;
	}	
	
	double gather(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const 
	{
			
	    	
	}

	
	void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total)
    	{
		vertex_data v=vertex.parse<vertex_data>();
		
		if(is_doc(vertex))
		{
			for (int k = 0; k < env_inst.NTOPICS; k++)
			{ 
				env_inst.theta[k2id(v.id)][k] += (v.n[k] + env_inst.ALPHA) / (env_inst.ndsum[k2id(v.id)] + env_inst.NTOPICS * env_inst.ALPHA);
			}
		}
		else
		{		
			for (int k = 0; k < env_inst.NTOPICS; k++) 
				env_inst.phi[k][v.id] += (v.n[k] + env_inst.BETA) / (env_inst.nwsum[k] + env_inst.NWORDS * env_inst.BETA);
	    	}
	}
	
	edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
		return saedb::NO_EDGES;
	}

	
	 void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const
	{
	}
};







void  ReadDocument()// every row of file is a document
{
	string s;
	ifstream fin(env_inst.INPUT_DOC);
	ofstream fout(env_inst.OUTPUT_TL);
	vector<string> components;
	vector<string> titles;
	vector<string> words;
	vector<string> authors;	
	//read the separator
	string l_sep;
	string w_sep,d_sep;
	int w_pos,d_pos;
	int w_start,d_start;
	ifstream fsep(env_inst.INPUT_SEP);
	while(getline(fsep,s)!=NULL)
	{
		int i;
		if(s.c_str()[0]=='l')
		{
			
			l_sep=s.substr(1,s.length()-1);
			
		}
		else if(s.c_str()[0]=='w')
		{
			w_pos=s.c_str()[1]-'0';
			i=2;
			while(s.c_str()[i] && s.c_str()[i]!='x')
				i++;
			w_sep=s.substr(2,i-2);
			w_start=s.c_str()[i+1]-'0';
			
		}
		else if(s.c_str()[0]=='d')
		{	
			d_pos=s.c_str()[1]-'0';
			i=2;
			while(s.c_str()[i] && s.c_str()[i]!='x')
				i++;
			d_sep=s.substr(2,i-2);
			d_start=s.c_str()[i+1]-'0';
			
		}
	}
	fsep.close();
	//read separator done
	while(getline(fin,s)!=NULL)
	{
		components = line_tokenize(s,l_sep);
		if(components.size()>d_pos)	
			titles = component_tokenize(components[d_pos],d_sep);
		if(components.size()>w_pos)
			words = component_tokenize(components[w_pos],w_sep);		
		//processing documents
		string title;	
		title=titles[d_start];
		fout<<title<<endl;
		int did = env_inst.docMap.size();
		env_inst.docMap.insert(make_pair(title,did));
		env_inst.docList.push_back(title);	
		
	       //processing words
		vector<int> vec;
		for(int i=w_start; i<words.size(); i++)
		{
			int wid; 
			map<string, int>::iterator it = env_inst.dict.find(words[i]);
			if(it==env_inst.dict.end())
			{
				wid = env_inst.dict.size();
				env_inst.dict.insert(make_pair(words[i],wid));
				env_inst.wordList.push_back(words[i]);
			}
			else
				wid=it->second;
			vec.push_back(wid);			
		}
		env_inst.wordlist.push_back(vec);	
	}
	
	env_inst.NWORDS=env_inst.wordList.size();
	env_inst.NDOCS=env_inst.docList.size();
	printf("read file number: ");cout<<env_inst.NDOCS<<endl;
	fin.close();
	fout.close();
	
	
}



void BuildLdaGraph(string file)
{
	sae::io::GraphBuilder<int> b;
        b.AddVertexDataType("LdaVertex");
        b.AddEdgeDataType("LdaEdge");
	vertex_data vd[env_inst.NDOCS];
	for(int i=0;i<env_inst.NDOCS;i++)
	{
		vd[i].id=id2k(i);
		vd[i].topic_num=env_inst.NTOPICS;
		vd[i].AllocHeap();
		b.AddVertex(vd[i].id,vd[i],"LdaVertex");
	}
	vertex_data vw[env_inst.NWORDS];
	for(int i=0;i<env_inst.NWORDS;i++)
	{
		vw[i].id=i;
		vw[i].topic_num=env_inst.NTOPICS;
		vw[i].AllocHeap();
		b.AddVertex(vw[i].id,vw[i],"LdaVertex");
	}
	int eid=0;
	for(int i=0;i<env_inst.NDOCS;i++)
	{
		for(int j=0;j<env_inst.wordlist[i].size();j++)
			b.AddEdge(vd[i].id,vw[env_inst.wordlist[i][j]].id,edge_data(eid++),"LdaEdge");
	}
	system("mkdir graph");
	system("mkdir ./graph/graph_lda");
	b.Save(file.c_str());
}
void GetDistribution()
{
	for(int i=0;i<env_inst.NDOCS;i++)
		for(int k=0;k<env_inst.NTOPICS;k++)
			env_inst.theta[i][k]/=env_inst.numstats;
	for(int k=0;k<env_inst.NTOPICS;k++)
		for(int v=0;v<env_inst.NWORDS;v++)
			env_inst.phi[k][v]/=env_inst.numstats;
}

void OutputTheta(string s)
{
	vector<int> topic2docs[env_inst.NTOPICS];

	ofstream fout(s.c_str());
	int i,j;
	for(i=0; i<env_inst.NDOCS; i++)
	{
		vector<pair<int ,double>> vec;
		double sum=0;
		for(j=0;j<env_inst.NTOPICS; j++)
		{
			sum+=env_inst.theta[i][j];
		}
		
		for(j=0;j<env_inst.NTOPICS;j++)
		{
			env_inst.theta[i][j]/=sum;
			fout<<env_inst.theta[i][j]<<" ";
			vec.push_back(make_pair(j,env_inst.theta[i][j]));
		}
		fout<<endl;
		
		sort_pair(vec);
		
		topic2docs[vec[0].first].push_back(i);
		
	}
	fout.close();
	fout.clear();

	fout.open(env_inst.OUTPUT_TD);
	for(int i=0;i<env_inst.NTOPICS;i++)
	{
		fout<<i<<"#"<<topic2docs[i].size()<<"#";
		for(int j=0;j<topic2docs[i].size();j++)
			fout<<topic2docs[i][j]<<" ";
		fout<<endl;
	}
	fout.close();

}

void OutputPhi(string s)
{
	ofstream fout(s.c_str());
	int i,j;
	for(i=0; i<env_inst.NTOPICS; i++)
	{
		fout<<i<<" ";
		vector<pair<int ,double>> vec;
		for(j=0;j<env_inst.NWORDS; j++)
		{
			vec.push_back(make_pair(j,env_inst.phi[i][j]));
		}
		sort_pair(vec);
		
		for(j=0;j<env_inst.NWORDS && j<env_inst.NKW;j++)
		{
			fout<<env_inst.wordList[vec[j].first]<<"#";
		}
		fout<<endl;
	}
	fout.close();
}
void OutputLda()
{
	system("mkdir output");
	system("mkdir ./output/output_lda");
	OutputTheta(env_inst.OUTPUT_TM);
	OutputPhi(env_inst.OUTPUT_WD);
}
void run_lda()
{
	cout << "Start LDA Module..." << endl;
	ReadDocument();
	cout<<"Reading Document Done."<<endl;
	
	env_inst.AllocHeap();
	cout<<env_inst<<endl;
	
	BuildLdaGraph("./graph/graph_lda/graph_lda");
        cout << "Generating Graph Data Done. " << endl;
    	
	graph_type lda_graph;
	lda_graph.load_format("./graph/graph_lda/graph_lda");
	cout <<"Loading Graph Done."<<endl;
	
	
	cout<<"LDA Start..."<<endl;	
	saedb::IEngine<lda_init> *engine_init = new saedb::EngineDelegate<lda_init>(lda_graph);	
	engine_init->signalAll();
	engine_init->start();
	delete engine_init;
	cout<<"Init done"<<endl;
	
	
	for(int i=0;i<env_inst.NROUNDS;i++)
	{
		cout<<"ROUNDS "<<i+1<<" is running!"<<endl;
		saedb::IEngine<lda_gibbs> *engine_gibbs = new saedb::EngineDelegate<lda_gibbs>(lda_graph);
		engine_gibbs->signalAll();			
		engine_gibbs->start();
		delete engine_gibbs;
		if ((i > env_inst.BURN_IN) && (env_inst.SAMPLE_LAG > 0) && (i % env_inst.SAMPLE_LAG == 0)) {
			saedb::IEngine<lda_update> *engine_update = new saedb::EngineDelegate<lda_update>(lda_graph);
			engine_update->signalAll();			
			engine_update->start();
			delete engine_update;
			env_inst.numstats++;
        	}
	}
	
	GetDistribution();
	cout<<"LDA Finished."<<endl;

	OutputLda();
	cout<<"Output Distribution Done."<<endl;
	
	//release resources
	for (auto i = 0; i < lda_graph.num_local_vertices(); i ++) {
	    	vertex_data v= lda_graph.vertex(i).parse<vertex_data>();
            	v.ReleaseHeap();
	}
	env_inst.ReleaseHeap();
        cout << "End LDA Module." << endl;
}
