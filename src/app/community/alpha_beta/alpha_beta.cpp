//#define DEGBUG
#include <iostream>
#include <algorithm>
#include <queue>
#include <cstdlib>
#include "sae_include.hpp"

#define ASSERT(X) {if (!(X)) { printf("ERROR LINE %d\n", __LINE__); exit(0); }}

struct AlphaBetaType {
	int alpha;
	int beta;

	AlphaBetaType() {
		alpha = 0;
		beta = 0;
	}

	AlphaBetaType(int alpha, int beta) {
		this->alpha = alpha;
		this->beta = beta;
	}

	AlphaBetaType& operator+=(const AlphaBetaType& other) {
		this->alpha += other.alpha;
		this->beta += other.beta;
		return *this;
	}
};

struct VertexDataType {
	saedb::vertex_id_type id;
	bool isSeed;
	bool inside;
	bool changed;
	AlphaBetaType alphaBeta;
	//store the neighboring vertices in A, B seth
	vector<saedb::vertex_id_type> abSet;
	set<saedb::vertex_id_type> neighbor_set;

	VertexDataType() {
		id = -1;
		isSeed = false;
		inside = false;
		changed = true;
	}
};

struct ABSetType {
	bool found;
	vector<VertexDataType*> aSet;
	vector<VertexDataType*> bSet;

	ABSetType() :
			found(false) {
	}
};

struct GatherDataType {
	AlphaBetaType alphaBeta;
	set<saedb::vertex_id_type> neighbor_set;

	GatherDataType& operator+=(const GatherDataType& other){
		this->alphaBeta += other.alphaBeta;
		this->neighbor_set.insert(other.neighbor_set.begin(), other.neighbor_set.end());
		return *this;
	}
};



//determine alpha_beta community size
int K = 100;
int* samples;

int stage;

//define graph type
typedef float edge_data_type;
typedef GatherDataType gather_type;
typedef saedb::sae_graph<VertexDataType, edge_data_type> graph_type;

//caculate current global alpha beta
class AlphaBeta: public saedb::IAlgorithm<graph_type, gather_type> {
private:
	bool init_neighbor_set;
	bool changed;
	//int stage;//0:caculate global alpha beta, 1:

public:
	//random choose a subset of K vertices
	void init(icontext_type& context, vertex_type& vertex) {
		vertex.data().id = vertex.id();
	}

	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const{
		if(vertex.data().changed){
			return saedb::ALL_EDGES;
		}else{
			return saedb::NO_EDGES;
		}

	}

	//compute alpha & beta value for each vertex
	gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const{
		GatherDataType gather_data;// = new gather_type();
		vertex_type other = edge.source().id() != vertex.id() ? edge.source() : edge.target();
		switch(stage){
		case 1:
			if(init_neighbor_set){
				gather_data.neighbor_set.insert(other.id());
			}
			if (other.data().inside) {
				gather_data.alphaBeta.alpha = 0;
				gather_data.alphaBeta.beta = 1;
			} else {
				gather_data.alphaBeta.alpha = 1;
				gather_data.alphaBeta.beta = 0;
			}
			break;
		case 2:


		}

		return gather_data;
	}

	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total) {
		switch(stage){
		case 1:
			if(init_neighbor_set){
				vertex.data().neighbor_set = total.neighbor_set;
			}
			vertex.data().alphaBeta = total.alphaBeta;

		}

	}

	edge_dir_type scatter_edges(icontext_type& context,
			const vertex_type& vertex) const{
		if(vertex.data().changed){
			return saedb::ALL_EDGES;
		}else{
			return saedb::NO_EDGES;
		}

	}

	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const{
		switch(stage){
		case 1:
			if(init_neighbor_set) init_neighbor_set = false;
			vertex_type other = edge.source().id() != vertex.id() ? edge.source() : edge.target();
		}


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
		if(inside){
			this->beta = std::min(this->beta, other.beta);
		}else{
			this->alpha = std::max(this->alpha, other.alpha);
		}
		return *this;
	}

};

alpha_beta_type global_alpha_beta_aggregator(const graph_type::vertex_type& vertex){
	return alpha_beta_type(vertex.data().alphaBeta.alpha, vertex.data().alphaBeta.beta, vertex.data().inside);
}

alpha_beta_type global_alpha_beta;

struct find_a_b_pair{
	queue<saedb::vertex_id_type> a_set;
	queue<saedb::vertex_id_type> b_set;
	bool finished;

	find_a_b_pair(saedb::vertex_id_type vid, bool inside, int alpha, int beta){
		finished = false
		if(inside)
			if(beta == global_alpha_beta.beta)
				b_set.push(vid);
		else
			if(alpha == global_alpha_beta.alpha)
				a_set.push(vid);
	}

	find_a_b_pair& operator+=(const find_a_b_pair& other){
		if(!other.finished){
			if(a_set.size() > 0){
				while(a_set.size() > 0){
					other.a_set.push(this->a_set.front());
					this->a_set.pop();
				}
				if(other.b_set.size() > 0)
					other.finished = true;
			}
			if(b_set.size() > 0){
				while(b_set.size() > 0){
					other.b_set.push(this->b_set.front());
					this->b_set.pop();
				}
				if(other.a_set.size() > 0)
					other.finished = true;
			}
		}
		*this = other;
		return *this;
	}
};

find_a_b_pair find_a_b_pair_aggregator(const graph_type::vertex_type& vertex){
	return find_a_b_pair(vertex.id(), vertex.data().inside, vertex.data().alphaBeta.alpha, vertex.data().alphaBeta.beta);
}

find_a_b_pair global_a_b_pair;

struct find_a_b_pair_non_neighbor{
	queue<saedb::vertex_id_type> a_set;
	queue<saedb::vertex_id_type> b_set;
	saedb::vertex_id_type a;
	saedb::vertex_id_type b;
	set<saedb::vertex_id_type> neighbor_set;
	bool finished;

	find_a_b_pair_non_neighbor(saedb::vertex_id_type vid, bool inside, int alpha, int beta, set<saedb::vertex_id_type>& neighbor_set){
		a = -1;
		b = -1;
		finished = false;
		this->neighbor_set = neighbor_set;
		if(inside)
			if(beta == global_alpha_beta.beta)
				b_set.push(vid);
		else
			if(alpha == global_alpha_beta.alpha)
				a_set.push(vid);
	}

	find_a_b_pair_non_neighbor& operator+=(const find_a_b_pair_non_neighbor& other){
		if(!other.finished){

			if(a_set.size() > 0){
				for(auto vid : other.b_set){
					if(neighbor_set.find(vid) == neighbor_set.end()){
						other.finished = true;
						other.a = a_set.front();
						other.b = vid;
					}
				}
			}
			if(b_set.size() > 0){
				for(auto vid : other.a_set){
					if(neighbor_set.find(vid) == neighbor_set.end()){
						other.finished = true;
						other.a = vid;
						other.b = b_set.front();
					}
				}
			}
		}
		*this = other;
		return *this;
	}

	static find_a_b_pair_non_neighbor aggregator(const graph_type::vertex_type& vertex){
		return find_a_b_pair_non_neighbor(vertex.id(), vertex.data().inside, vertex.data().alphaBeta.alpha, vertex.data().alphaBeta.beta, vertex.data().neighbor_set);
	}
};



find_a_b_pair_non_neighbor global_find_a_b_pair_non_neighbor;

struct find_a{
	saedb::vertex_id_type a;
	bool finished;

	find_a(saedb::vertex_id_type vid, bool inside, int alpha){
		a = -1;
		if(inside && alpha == global_alpha_beta.alpha)
			a = vid;
		finished = false;
	}

	find_a& operator+=(const find_a& other){
		if(!other.finished){
			if(a > -1){
				other.a = a;
				other.finished = true;
			}
		}
		*this = other;
		return *this;
	}

	static find_a aggregator(const graph_type::vertex_type& vertex){
		return find_a(vertex.id(), vertex.data().inside, vertex.data().alphaBeta.alpha);
	}
};


struct find_b{
	saedb::vertex_id_type b;
	bool finished;

	find_b(saedb::vertex_id_type vid, bool inside, int beta){
		b = -1;
		if(inside && beta == global_alpha_beta.beta)
			b = vid;
		finished = false;
	}

	find_b& operator+=(const find_b& other){
		if(!other.finished){
			if(b > -1){
				other.b = b;
				other.finished = true;
			}
		}
		*this = other;
		return *this;
	}

	static find_b aggregator(const graph_type::vertex_type& vertex){
		return find_a(vertex.id(), vertex.data().inside, vertex.data().alphaBeta.alpha);
	}
};

struct find_a_set{
	queue<saedb::vertex_id_type> a_set;

	find_a_set(saedb::vertex_id_type vid, bool inside, int alpha){

	}

	find_a_set& operator+=(const find_a_set& other){
		while(a_set.size() > 0){
			other.a_set.push(a_set.front());
			a_set.pop();
		}
		*this = other;
		return *this;
	}

	static find_a_set aggregator(const graph_type::vertex_type& vertex){
		if(!vertex.data().inside && vertex.data().alphaBeta.alpha == global_alpha_beta.alpha)
			vertex.data().inside = true;
		return find_a_set(vertex.id(), vertex.data().inside, vertex.data().alphaBeta.alpha);
	}
};

void alpha_beta_community_detection(saedb::IEngine<AlphaBeta>* engine, graph_type& graph){
	//caculate local alpha beta
	engine->signalAll();
	//caculate global alpha beta
	global_alpha_beta = engine->map_reduce_vertices<alpha_beta_type>(global_alpha_beta_aggregator);

	while(global_alpha_beta.alpha >= global_alpha_beta.beta){

		//swapping
		while(global_alpha_beta.alpha > global_alpha_beta.beta){
			//perform swapping
			global_a_b_pair = engine->map_reduce_vertices<alpha_beta_type>(find_a_b_pair_aggregator);
			auto vertex_a = graph.vertex(global_a_b_pair.a_set.front).data();
			auto vertex_b = graph.vertex(global_a_b_pair.b_set.front).data();
			global_a_b_pair.a_set.pop();
			global_a_b_pair.b_set.pop();
			vertex_a.inside = true;
			vertex_b.inside = false;
			std::vector<saedb::vertex_id_type>signal_vertices;
			signal_vertices.push_back(vertex_a.id());
			signal_vertices.push_back(vertex_b.id());
			for(auto vid : vertex_a.data().neighbor_set)
				signal_vertices.push_back(vid);
			for(auto vid : vertex_b.data().neighbor_set)
				signal_vertices.push_back(vid);
			//caculate global alpha beta
			engine->signalVertices(signal_vertices);
		}
		find_a_b_pair_non_neighbor find_a_b_pair_non_neighbor_result = engine->map_reduce_vertices<alpha_beta_type>(find_a_b_pair_non_neighbor::aggregator);
		if(find_a_b_pair_non_neighbor_result.finished){
			auto vertex_a = graph.vertex(find_a_b_pair_non_neighbor_result.a).data();
			auto vertex_b = graph.vertex(find_a_b_pair_non_neighbor_result.b).data();
			vertex_a.inside = true;
			vertex_b.inside = false;
			for(auto vid : vertex_a.data().neighbor_set)
				signal_vertices.push_back(vid);
			for(auto vid : vertex_b.data().neighbor_set)
				signal_vertices.push_back(vid);
			//caculate global alpha beta
			engine->signalVertices(signal_vertices);
		}
		else{
			find_a find_a_result = engine->map_reduce_vertices<alpha_beta_type>(find_a::aggregator);
			if(find_a_result.finished){
				auto vertex_a = graph.vertex(find_a_result.a).data();
				vertex_a.inside = true;
				for(auto vid : vertex_a.data().neighbor_set)
					signal_vertices.push_back(vid);
				//caculate global alpha beta
				engine->signalVertices(signal_vertices);
			}
			else{
				find_b find_b_result = engine->map_reduce_vertices<alpha_beta_type>(find_b::aggregator);
				if(find_b_result.finished){
					auto vertex_b = graph.vertex(find_b_result.b).data();
					vertex_b.inside = false;
					for(auto vid : vertex_b.data().neighbor_set)
						signal_vertices.push_back(vid);
					//caculate global alpha beta
					engine->signalVertices(signal_vertices);
				}
				else{
					find_a_set find_a_set_result = engine->map_reduce_vertices<alpha_beta_type>(find_a_set::aggregator);

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

    std::string graph_path = "test_graph";

    // todo read graph_dir and format

    graph_type graph;
    graph.load_format(graph_path);

    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        std::cout << "v[" << i << "]: " << graph.vertex(i).data() << endl;
    }

    std::cout << "#vertices: "
    << graph.num_vertices()
    << " #edges:"
    << graph.num_edges() << std::endl;

	saedb::IEngine<AlphaBeta> *engine = new saedb::EngineDelegate<AlphaBeta>(graph);

	alpha_beta_community_detection(engine, graph);


}
