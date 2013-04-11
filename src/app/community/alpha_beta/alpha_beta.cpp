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
	int alpha_beta;

	vertex_data_type() {
		alpha_beta = 0;
		is_seed = false;
		inside = false;
		changed = true;
	}
};


struct gather_data_type {
	int alpha_beta;

	gather_data_type& operator+=(const gather_data_type& other){
		this->alpha_beta += other.alpha_beta;
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
		if (other.data().inside)
			gather_data.alpha_beta = 1;
		else
			gather_data.alpha_beta = 0;
		std::cout << vertex.id()<< vertex.data().inside << other.id() << other.data().inside <<" alpha_beta "
				<< gather_data.alpha_beta << std::endl;
		return gather_data;
	}

	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total) {
		std::cout<<"id:"<<vertex.id()<<" "<<total.alpha_beta<<std::endl;
		vertex.data().alpha_beta = total.alpha_beta;
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

	alpha_beta_type(): alpha(-1), beta(-1){ }
	alpha_beta_type(int alpha_beta, bool inside){
		alpha = -1;
		beta = -1;
		if(inside){
			beta = alpha_beta;
		}else{
			alpha = alpha_beta;
		}
	}

	alpha_beta_type& operator+=(const alpha_beta_type& other){
		if(other.beta > -1){
			if(beta > -1){
				beta = std::min(beta, other.beta);
			}else{
				beta = other.beta;
			}
		}else if(other.alpha > -1){
			if(alpha > -1){
				alpha = std::max(alpha, other.alpha);
			}else{
				alpha = other.alpha;
			}
		}
		std::cout<<" alpha:"<<alpha<<" beta:"<<beta<<endl;
		return *this;
	}

	static alpha_beta_type aggregator(graph_type::vertex_type& vertex){
		std::cout<<"id:"<<vertex.id()<<" inside: "<<vertex.data().inside<<" alpha:"<<vertex.data().alpha_beta<<std::endl;
		return alpha_beta_type(vertex.data().alpha_beta, vertex.data().inside);
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

	find_a_b_pair(saedb::vertex_id_type vid, bool inside, int alpha_beta){
		finished = false;
		a = -1;
		b = -1;
		if(inside && alpha_beta == global_alpha_beta->beta)
			b = vid;
		else if(!inside && alpha_beta == global_alpha_beta->alpha)
			a = vid;
		std::cout<<"id: "<<vid<<"a: "<<this->a<<" b: "<<this->b<<" invalid: "<<saedb::INVALID_ID<<std::endl;
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
		return find_a_b_pair(vertex.id(), vertex.data().inside, vertex.data().alpha_beta);
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

	find_a_b_pair_non_neighbor(saedb::vertex_id_type vid, bool inside, int alpha_beta, const set<saedb::vertex_id_type>& neighbor_set){
		a = -1;
		b = -1;
		finished = false;
		this->neighbor_set = neighbor_set;
		if(inside && alpha_beta == global_alpha_beta->beta){
			b = vid;
			b_set.push_back(vid);
		}
		if(!inside && alpha_beta == global_alpha_beta->alpha){
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
		return find_a_b_pair_non_neighbor(vertex.id(), vertex.data().inside, vertex.data().alpha_beta, neighbor_set);
	}
};

struct find_a{
	saedb::vertex_id_type a;
	bool finished;

	find_a(){
		a = -1;
		finished = false;
	}

	find_a(saedb::vertex_id_type vid){
		a = vid;
		finished = false;
	}

	find_a& operator+=(const find_a& other){
		if(!finished){
			if(other.a != saedb::INVALID_ID){
				a = other.a;
				finished = true;
			}
		}
		return *this;
	}

	static find_a aggregator(graph_type::vertex_type& vertex){
		if(!vertex.data().inside && vertex.data().alpha_beta == global_alpha_beta->alpha){
			bool flag = true;
			for (auto ei = vertex.in_edges(); ei->Alive(); ei->Next()){
				vertex_data_type* vd = (vertex_data_type*) ei->Target()->Data();
				if(!vd->inside && vd->alpha_beta == global_alpha_beta->alpha){
					flag = false;
					break;
				}
			}
			for (auto ei = vertex.out_edges(); ei->Alive(); ei->Next()){
				vertex_data_type* vd = (vertex_data_type*) ei->Source()->Data();
				if(!vd->inside && vd->alpha_beta == global_alpha_beta->alpha){
					flag = false;
					break;
				}
			}
			if(flag)
				return find_a(vertex.id());
			else
				return find_a(-1);
		}
		else{
			return find_a(-1);
		}
	}
};


struct find_b{
	saedb::vertex_id_type b;
	bool finished;

	find_b(saedb::vertex_id_type vid){
		b = vid;
		finished = false;
	}

	find_b(){
		b = -1;
		finished = false;
	}

	find_b& operator+=(const find_b& other){
		if(!finished){
			if(other.b != saedb::INVALID_ID){
				b = other.b;
				finished = true;
			}
		}
		return *this;
	}

	static find_b aggregator(graph_type::vertex_type& vertex){
		if(vertex.data().inside && vertex.data().alpha_beta == global_alpha_beta->beta){
			bool flag = true;
			for (auto ei = vertex.in_edges(); ei->Alive(); ei->Next()){
				vertex_data_type* vd = (vertex_data_type*) ei->Target()->Data();
				if(vd->inside && vd->alpha_beta == global_alpha_beta->beta){
					flag = false;
					break;
				}
			}
			for (auto ei = vertex.out_edges(); ei->Alive(); ei->Next()){
				vertex_data_type* vd = (vertex_data_type*) ei->Source()->Data();
				if(vd->inside && vd->alpha_beta == global_alpha_beta->beta){
					flag = false;
					break;
				}
			}
			if(flag)
				return find_b(vertex.id());
			else
				return find_b(-1);
		}
		else{
			return find_b(-1);
		}
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
		if(!vertex.data().inside && vertex.data().alpha_beta == global_alpha_beta->alpha)
			vertex.data().inside = true;
		return find_a_set(vertex.id(), vertex.data().inside, vertex.data().alpha_beta);
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
			engine->signalVertices(signal_vertices);
			engine->start();
			alpha_beta_result = engine->map_reduce_vertices<alpha_beta_type>(alpha_beta_type::aggregator);
			global_alpha_beta = &alpha_beta_result;
		}
		cout<<"find_a_b_pair_non_neighbor"<<endl;
		find_a_b_pair_non_neighbor find_a_b_pair_non_neighbor_result = engine->map_reduce_vertices<find_a_b_pair_non_neighbor>(find_a_b_pair_non_neighbor::aggregator);
		if(find_a_b_pair_non_neighbor_result.finished){
			cout<<"a: "<<find_a_b_pair_non_neighbor_result.a<<" b: "<<find_a_b_pair_non_neighbor_result.b<<endl;
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
		break;

	}
}


int main() {

    std::string graph_path = "alpha_beta_graph";

    // todo read graph_dir and format

    graph_type graph;
    graph.load_format(graph_path);
    cout<<"engine"<<endl;
    cout<<graph.num_local_vertices()<<endl;
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        std::cout << "v[" << i << "]: " << graph.vertex(i).data().inside <<  endl;
    }

	saedb::IEngine<AlphaBeta> *engine = new saedb::EngineDelegate<AlphaBeta>(graph);
	cout<<"alpha_beta_community_detection"<<endl;
	alpha_beta_community_detection(engine, graph);


}
