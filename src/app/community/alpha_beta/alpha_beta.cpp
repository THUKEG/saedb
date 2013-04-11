//#define DEGBUG
#include <iostream>
#include <algorithm>
#include <queue>
#include <cstdlib>
#include "sae_include.hpp"

#define ASSERT(X) {if (!(X)) { printf("ERROR LINE %d\n", __LINE__); exit(0); }}

struct vertex_data_type {
	bool is_seed;
	bool inside;
	bool changed;
	int alpha;
	int beta;

	vertex_data_type() {
		alpha = 0;
		beta = 0;
		is_seed = false;
		inside = false;
		changed = true;
	}
};


struct gather_data_type {
	int alpha;
	int beta;

	gather_data_type& operator+=(const gather_data_type& other){
		this->alpha += other.alpha;
		this->beta += other.beta;
		return *this;
	}
};



//determine alpha_beta community size
int K = 100;
int* samples;

int stage;

//define graph type
typedef float edge_data_type;
typedef gather_data_type gather_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

//caculate current global alpha beta
class AlphaBeta: public saedb::IAlgorithm<graph_type, gather_type> {
private:
	bool changed;

public:
	AlphaBeta(){
		changed =true;
		cout<<"init vertex program"<<endl;
	}
	//random choose a subset of K vertices
	void init(icontext_type& context, vertex_type& vertex) {
	}

	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const{
			return saedb::ALL_EDGES;
	}

	//compute alpha & beta value for each vertex
	gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const{
		gather_data_type gather_data;
		vertex_type other = edge.source().id() != vertex.id() ? edge.source() : edge.target();
		if (other.data().inside) {
			gather_data.alpha = 0;
			gather_data.beta = 1;
		} else {
			gather_data.alpha = 1;
			gather_data.beta = 0;
		}
		std::cout << vertex.id()<< vertex.data().inside << other.id() << other.data().inside <<" alpha " << gather_data.alpha << " " << "beta " << gather_data.beta << std::endl;
		return gather_data;
	}

	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total) {
		std::cout<<"id:"<<vertex.id()<<" "<<total.alpha<<" "<<total.beta<<std::endl;
		vertex.data().alpha = total.alpha;
		vertex.data().beta = total.beta;
	}

	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const{
			return saedb::NO_EDGES;
	}

	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const{
	}

};

struct alpha_beta_type{
	int alpha;
	int beta;
	bool inside;

	alpha_beta_type(): alpha(-1), beta(-1), inside(false) { }
	alpha_beta_type(int alpha, int beta, bool inside){
		this->alpha = alpha;
		this->beta = beta;
		this->inside = inside;
	}

	alpha_beta_type& operator+=(const alpha_beta_type& other){
		std::cout<<"inside:"<<inside<<" alpha:"<<alpha<<" beta:"<<beta<<endl;
		if(inside){
			this->beta = std::min(this->beta, other.beta);
		}else{
			this->alpha = std::max(this->alpha, other.alpha);
		}
		std::cout<<"inside:"<<inside<<" alpha:"<<alpha<<" beta:"<<beta<<endl;
		return *this;
	}

	static alpha_beta_type aggregator(graph_type::vertex_type& vertex){
		std::cout<<"id:"<<vertex.id()<<" inside: "<<vertex.data().inside<<" alpha:"<<vertex.data().alpha<<" beta:"<<vertex.data().beta<<std::endl;
		return alpha_beta_type(vertex.data().alpha, vertex.data().beta, vertex.data().inside);
	}
};

alpha_beta_type* global_alpha_beta;

struct find_a_b_pair{
	saedb::vertex_id_type a;
	saedb::vertex_id_type b;
	bool finished;

	find_a_b_pair(){
		a = -1;
		b = -1;
		finished = false;
	}

	find_a_b_pair(saedb::vertex_id_type vid, bool inside, int alpha, int beta){
		finished = false;
		a = -1;
		b = -1;
		if(inside && beta == global_alpha_beta->beta)
			b = vid;
		else if(!inside && alpha == global_alpha_beta->alpha)
			a = vid;
		std::cout<<"a: "<<this->a<<" b: "<<this->b<<" invalid: "<<saedb::INVALID_ID<<std::endl;
	}

	find_a_b_pair& operator+=(const find_a_b_pair& other){
		std::cout<<"before a: "<<this->a<<" b: "<<this->b<<std::endl;
		if(!other.finished && !finished){
			if(other.b != saedb::INVALID_ID){
				b = other.b;
				if(a != saedb::INVALID_ID){
					finished = true;
				}
			}
			if(other.a != saedb::INVALID_ID){
				a = other.a;
				if(b != saedb::INVALID_ID){
					finished = true;
				}
			}
		}
		std::cout<<"after a: "<<this->a<<" b: "<<this->b<<std::endl;
		return *this;
	}

	static find_a_b_pair aggregator(graph_type::vertex_type& vertex){
		return find_a_b_pair(vertex.id(), vertex.data().inside, vertex.data().alpha, vertex.data().beta);
	}
};

find_a_b_pair* global_a_b_pair;

struct find_a_b_pair_non_neighbor{
	vector<saedb::vertex_id_type> a_set;
	vector<saedb::vertex_id_type> b_set;
	saedb::vertex_id_type a;
	saedb::vertex_id_type b;
	set<saedb::vertex_id_type> neighbor_set;
	bool finished;

	find_a_b_pair_non_neighbor(){
		a = -1;
		b = -1;
		finished = false;
	}

	find_a_b_pair_non_neighbor(saedb::vertex_id_type vid, bool inside, int alpha, int beta, const set<saedb::vertex_id_type>& neighbor_set){
		a = -1;
		b = -1;
		finished = false;
		this->neighbor_set = neighbor_set;
		if(inside && beta == global_alpha_beta->beta){
			b = vid;
			b_set.push_back(vid);
		}
		else if(alpha == global_alpha_beta->alpha){
			a = vid;
			a_set.push_back(vid);
		}
	}

	find_a_b_pair_non_neighbor& operator+=(const find_a_b_pair_non_neighbor& other){
		if(!other.finished && !finished){
			if(a != saedb::INVALID_ID){
				for(auto vid : other.b_set){
					if(neighbor_set.find(vid) == neighbor_set.end()){
						finished = true;
						b = vid;
					}
				}
				if(!finished){
					for(auto vid : other.a_set){
						a_set.push_back(vid);
					}
				}
			}
			if(b != saedb::INVALID_ID){
				for(auto vid : other.a_set){
					if(neighbor_set.find(vid) == neighbor_set.end()){
						finished = true;
						a = vid;
					}
				}
				if(!finished){
					for(auto vid : other.b_set){
						b_set.push_back(vid);
					}
				}
			}
		}
		return *this;
	}

	static find_a_b_pair_non_neighbor aggregator(graph_type::vertex_type& vertex){
		std::set<saedb::vertex_id_type> neighbor_set;
		for (auto ei = vertex.in_edges(); ei->Alive(); ei->Next())
			neighbor_set.insert(ei->SourceId());
		for (auto ei = vertex.out_edges(); ei->Alive(); ei->Next())
			neighbor_set.insert(ei->TargetId());
		return find_a_b_pair_non_neighbor(vertex.id(), vertex.data().inside, vertex.data().alpha, vertex.data().beta, neighbor_set);
	}
};

struct find_a{
	saedb::vertex_id_type a;
	bool finished;

	find_a(){
		a = -1;
		finished = false;
	}

	find_a(saedb::vertex_id_type vid, bool inside, int alpha){
		a = -1;
		if(inside && alpha == global_alpha_beta->alpha)
			a = vid;
		finished = false;
	}

	find_a& operator+=(const find_a& other){
		if(!other.finished && finished){
			if(other.a != saedb::INVALID_ID){
				a = other.a;
				finished = true;
			}
		}
		return *this;
	}

	static find_a aggregator(graph_type::vertex_type& vertex){
		return find_a(vertex.id(), vertex.data().inside, vertex.data().alpha);
	}
};


struct find_b{
	saedb::vertex_id_type b;
	bool finished;

	find_b(saedb::vertex_id_type vid, bool inside, int beta){
		b = -1;
		if(inside && beta == global_alpha_beta->beta)
			b = vid;
		finished = false;
	}

	find_b(){
		b = -1;
		finished = false;
	}

	find_b& operator+=(const find_b& other){
		if(!other.finished && finished){
			if(other.b != saedb::INVALID_ID){
				b = other.b;
				finished = true;
			}
		}
		return *this;
	}

	static find_b aggregator(graph_type::vertex_type& vertex){
		return find_b(vertex.id(), vertex.data().inside, vertex.data().alpha);
	}
};

struct find_a_set{
	vector<saedb::vertex_id_type> a_set;

	find_a_set(){}

	find_a_set(saedb::vertex_id_type vid, bool inside, int alpha){
		if(!inside && alpha == global_alpha_beta->alpha){
			a_set.push_back(vid);
		}
	}

	find_a_set& operator+=(const find_a_set& other){
		for(auto vid : other.a_set){
			a_set.push_back(vid);
		}
		return *this;
	}

	static find_a_set aggregator(graph_type::vertex_type& vertex){
		if(!vertex.data().inside && vertex.data().alpha == global_alpha_beta->alpha)
			vertex.data().inside = true;
		return find_a_set(vertex.id(), vertex.data().inside, vertex.data().alpha);
	}
};

void alpha_beta_community_detection(saedb::IEngine<AlphaBeta>* engine, graph_type& graph){
	//caculate local alpha beta
	cout<<"engine start"<<endl;
	engine->signalAll();
	engine->start();
	//caculate global alpha beta
	cout<<"alpha_beta"<<endl;
	auto alpha_beta_result = engine->map_reduce_vertices<alpha_beta_type>(alpha_beta_type::aggregator);
	global_alpha_beta = &alpha_beta_result;
	cout<<"alpha:"<<alpha_beta_result.alpha<<" "<<"beta:"<<alpha_beta_result.beta<<endl;
	while(global_alpha_beta->alpha >= global_alpha_beta->beta){
		//swapping
		while(global_alpha_beta->alpha > global_alpha_beta->beta){
			//perform swapping
			cout<<"find_a_b_pair"<<endl;
			auto find_a_b_pair_result = engine->map_reduce_vertices<find_a_b_pair>(find_a_b_pair::aggregator);
			cout<<find_a_b_pair_result.a<<endl;
			auto vertex_a = graph.vertex(find_a_b_pair_result.a);
			cout<<find_a_b_pair_result.b<<endl;
			auto vertex_b = graph.vertex(find_a_b_pair_result.b);
			vertex_a.data().inside = true;
			vertex_b.data().inside = false;
			std::vector<saedb::vertex_id_type>signal_vertices;
			signal_vertices.push_back(vertex_a.id());
			signal_vertices.push_back(vertex_b.id());
	        for (auto ei = vertex_a.in_edges(); ei->Alive(); ei->Next())
	        	signal_vertices.push_back(ei->SourceId());
			for (auto ei = vertex_a.out_edges(); ei->Alive(); ei->Next())
				signal_vertices.push_back(ei->TargetId());
			for (auto ei = vertex_b.in_edges(); ei->Alive(); ei->Next())
				signal_vertices.push_back(ei->SourceId());
			for (auto ei = vertex_b.out_edges(); ei->Alive(); ei->Next())
				signal_vertices.push_back(ei->TargetId());
			//caculate global alpha beta
			cout<<"caculate global alpha beta"<<endl;
			engine->signalVertices(signal_vertices);
		}
		cout<<"find_a_b_pair_non_neighbor"<<endl;
		find_a_b_pair_non_neighbor find_a_b_pair_non_neighbor_result = engine->map_reduce_vertices<find_a_b_pair_non_neighbor>(find_a_b_pair_non_neighbor::aggregator);
		if(find_a_b_pair_non_neighbor_result.finished){
			auto vertex_a = graph.vertex(find_a_b_pair_non_neighbor_result.a);
			auto vertex_b = graph.vertex(find_a_b_pair_non_neighbor_result.b);
			vertex_a.data().inside = true;
			vertex_b.data().inside = false;
			std::vector<saedb::vertex_id_type>signal_vertices;
			signal_vertices.push_back(vertex_a.id());
			signal_vertices.push_back(vertex_b.id());
	        for (auto ei = vertex_a.in_edges(); ei->Alive(); ei->Next())
	        	signal_vertices.push_back(ei->SourceId());
			for (auto ei = vertex_a.out_edges(); ei->Alive(); ei->Next())
				signal_vertices.push_back(ei->TargetId());
			for (auto ei = vertex_b.in_edges(); ei->Alive(); ei->Next())
				signal_vertices.push_back(ei->SourceId());
			for (auto ei = vertex_b.out_edges(); ei->Alive(); ei->Next())
				signal_vertices.push_back(ei->TargetId());
			//caculate global alpha beta
			engine->signalVertices(signal_vertices);
		}
		else{
			cout<<"find_a";
			find_a find_a_result = engine->map_reduce_vertices<find_a>(find_a::aggregator);
			if(find_a_result.finished){
				auto vertex_a = graph.vertex(find_a_result.a);
				vertex_a.data().inside = true;
				std::vector<saedb::vertex_id_type>signal_vertices;
				signal_vertices.push_back(vertex_a.id());
		        for (auto ei = vertex_a.in_edges(); ei->Alive(); ei->Next())
		        	signal_vertices.push_back(ei->SourceId());
				for (auto ei = vertex_a.out_edges(); ei->Alive(); ei->Next())
					signal_vertices.push_back(ei->TargetId());
				//caculate global alpha beta
				engine->signalVertices(signal_vertices);
			}
			else{
				cout<<"find_b";
				find_b find_b_result = engine->map_reduce_vertices<find_b>(find_b::aggregator);
				if(find_b_result.finished){
					auto vertex_b = graph.vertex(find_b_result.b);
					vertex_b.data().inside = false;
					std::vector<saedb::vertex_id_type>signal_vertices;
					signal_vertices.push_back(vertex_b.id());
					for (auto ei = vertex_b.in_edges(); ei->Alive(); ei->Next())
						signal_vertices.push_back(ei->SourceId());
					for (auto ei = vertex_b.out_edges(); ei->Alive(); ei->Next())
						signal_vertices.push_back(ei->TargetId());
					//caculate global alpha beta
					engine->signalVertices(signal_vertices);
				}
				else{
					cout<<"find_a_set";
					find_a_set find_a_set_result = engine->map_reduce_vertices<find_a_set>(find_a_set::aggregator);
				}
			}
		}

	}
}


//int reservoirSample(int sample, int* samples, int size, int count) {
//	if (count < sample)
//		samples[count] = sample;
//	else if ((rand() % count) < size)
//		sample[rand() % size] = sample;
//	return ++count;
//}



int main() {
	//random choose a subset of K vertices
	//implemented reservoir sampling
//	int count = 0;
//	samples = new int[K];
//	int sample;
//	int i = 0;
//	srand(time(NULL));

    std::string graph_path = "alpha_beta_graph";

    // todo read graph_dir and format

    graph_type graph;
    graph.load_format(graph_path);
    cout<<"engine"<<endl;
    cout<<graph.num_local_vertices()<<endl;
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        std::cout << "v[" << i << "]: " << graph.vertex(i).data().inside <<  endl;
    }

//    std::cout << "#vertices: "
//    << graph.num_vertices()
//    << " #edges:"
//    << graph.num_edges() << std::endl;
	saedb::IEngine<AlphaBeta> *engine = new saedb::EngineDelegate<AlphaBeta>(graph);
	cout<<"alpha_beta_community_detection"<<endl;
	alpha_beta_community_detection(engine, graph);


}
