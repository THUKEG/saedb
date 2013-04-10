//#define DEGBUG
#include <iostream>
#include <cstdlib>
#include "sae_include.hpp"
//#include "sample_data.hpp"

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
	bool inCluster;
	bool changed;
	AlphaBetaType alphaBeta;
	//store the neighboring vertices in A, B seth
	vector<saedb::vertex_id_type> abSet;
	set<saedb::vertex_id_type> neighborSet;

	VertexDataType() {
		id = -1;
		isSeed = false;
		inCluster = false;
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
	set<saedb::vertex_id_type> neighborSet;

	GatherDataType& operator+=(const GatherDataType& other){
		this->alphaBeta += other.alphaBeta;
		this->neighborSet.insert(other.neighborSet.begin(), other.neighborSet.end());
		return *this;
	}
};

//define the aggregators
class AlphaBetaAggregator: public saedb::IAggregator {
private:
	AlphaBetaType accu;

public:
//	void init(void* i) {
//		accu = *((float*) i);
//	}

	void reduce(void* next) {
		accu.alpha = max(accu.alpha, ((AlphaBetaType*) next)->alpha);
		accu.beta = min(accu.beta, ((AlphaBetaType*) next)->beta);
	}

	void* data() const {
		return (void*) (&accu);
	}
};


//find a pair of non-neighboring vertices in A and B respectly
class ABSetAggregator: public saedb::IAggregator {
private:
	ABSetType abSet;

public:
	void init(void* i) {
		//abSet = new ABSetType();
	}

	void reduce(void* next) {
		VertexDataType* vertexData = (VertexDataType*)next;
		if (vertexData->inCluster) {
			for(auto _a : abSet.aSet){
				//found a pair
				if(vertexData->neighborSet.find(_a->id) == vertexData->neighborSet.end()){
					//swap
					vertexData->inCluster = false;
					_a->inCluster = true;
				}
			}
			abSet.bSet.push_back(vertexData);
		} else {
			abSet.aSet.push_back(vertexData);
		}
	}

	void* data() const {
		return (void*) (&abSet);
	}
};


//perform swapping alogrithm
class SwappingAggregator: public saedb::IAggregator {
private:
	bool a = false;
	bool b = false;

public:
	void init(void* i) {

	}

	void reduce(void* next) {
		VertexDataType* vertexData = (VertexDataType*)next;
		if(!a){
			if(!vertexData->inCluster){
				if
			}
		}else if(!b){
			if(vertexData->inCluster){

			}
		}
	}

	void* data() const{

	}
}


//determine alpha_beta community size
int K = 100;
int* samples;

//define graph type
typedef float edge_data_type;
typedef GatherDataType gather_type;
typedef saedb::sae_graph<VertexDataType, edge_data_type> graph_type;

//caculate current global alpha beta
class AlphaBeta: public saedb::IAlgorithm<graph_type, gather_type> {
private:
	bool initNeighborSet;
	bool changed;
	int stage;//0:caculate global alpha beta, 1:
public:

	//random choose a subset of K vertices
	void init(icontext_type& context, vertex_type& vertex) {
		vertex.data().id = vertex.id();
	}

	edge_dir_type gather_edges(icontext_type& context,
			const vertex_type& vertex) const{
		if(vertex.data().changed){
			return saedb::ALL_EDGES;
		}else{
			return saedb::NO_EDGES;
		}

	}

	//compute alpha & beta value for each vertex
	gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const{
		vertex_type other = edge.source().id() != vertex.id() ? edge.source() : edge.target();
		GatherDataType gather_data;// = new gather_type();
		if(initNeighborSet){
			gather_data.neighborSet.insert(other.id());
			//initNeighborSet = false;
		}
		if (other.data().inCluster) {
			gather_data.alphaBeta.alpha = 0;
			gather_data.alphaBeta.beta = 1;
		} else {
			gather_data.alphaBeta.alpha = 1;
			gather_data.alphaBeta.beta = 0;
		}
		return gather_data;
	}

	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total) {
		vertex.data().alphaBeta = total.alphaBeta;
		vertex.data().neighborSet = total.neighborSet;

//		saedb::IAggregator* alphaBetaAggerator = context.getAggregator("global_alpha_beta");
//		AlphaBetaType* alphaBeta = (AlphaBetaType*)alphaBetaAggerator->data();
//		if (alphaBeta->alpha > alphaBeta->beta) {
//			//swapping
//			if (vertex.data().inCluster) {
//				//in set B
//#ifdef DEBUG
//				ASSERT(vertex.data().alphaBeta.alpha <= globalAlphaBeta->data()->alpha);
//#endif
//				if (vertex.data().alphaBeta.alpha
//						== ((AlphaBetaType*) alphaBetaAggerator->data())->alpha) {
//					vertex.data().inCluster = false;
//				}
//			} else {
//				//in set A
//#ifdef DEBUG
//				ASSERT(vertex.data().alphaBeta.beta >= globalAlphaBeta->data()->beta);
//#endif
//				if (vertex.data().alphaBeta.beta
//						== ((AlphaBetaType*) alphaBetaAggerator->data())->beta) {
//					vertex.data().inCluster = true;
//				}
//			}
//			//end of swapping
//		} else if (alphaBeta->alpha == alphaBeta->beta) {
//
//		}

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
		vertex_type other = edge.source().id() != vertex.id() ? edge.source() : edge.target();
		if (vertex.data().inCluster) {
			if (other.data().inCluster) {
				vertex.data().abSet.push_back(other.id());
			}
		} else {
			if (!other.data().inCluster) {
				vertex.data().abSet.push_back(other.id());
			}
		}
		vertex.data().changed = false;
	}

	void aggerate(icontext_type& context, const vertex_type& vertex) {
		//caculate global alpha beta
		AlphaBetaAggregator* alphaBetaAggerator = (AlphaBetaAggregator*)context.getAggregator("global_alpha_beta");
		AlphaBetaType _alphaBeta = vertex.data().alphaBeta;
		alphaBetaAggerator->reduce(&_alphaBeta);

//		if (_alphaBeta.alpha > _alphaBeta.beta) {
//			context.signal(vertex);
//		} else if (_alphaBeta.alpha == _alphaBeta.beta) {
//			ABSetAggregator* abSetAggregator = (ABSetAggregator*)context.getAggregator("global_ab_set");
//			if(((ABSetType*)abSetAggregator->data())->found) return;
//			VertexDataType _vdata = vertex.data();
//			abSetAggregator->reduce(&_vdata);
//		} else {
//
//		}
	}
};


//int reservoirSample(int sample, int* samples, int size, int count) {
//	if (count < sample)
//		samples[count] = sample;
//	else if ((rand() % count) < size)
//		sample[rand() % size] = sample;
//	return ++count;
//}

graph_type sampleGraph() {
	//return LOAD_SAMPLE_GRAPH<graph_type>();
	return graph_type();
}

int main() {
	graph_type graph = sampleGraph();

	//random choose a subset of K vertices
	//implemented reservoir sampling
	int count = 0;
	samples = new int[K];
	int sample;
	int i = 0;
	srand(time(NULL));
	auto id_set = graph.vertex_ids;
	int index = 0;
	for (int vertex_id : id_set) {
		if (index < K)
			samples[index] = vertex_id;
		count = rand() % index;
		if (count < K) {
			samples[count] = vertex_id;
		}
		index += 1;
	}
	//aggregator instances
	AlphaBetaAggregator* globalAlphaBeta;
	ABSetAggregator* globalABSet;

	//caculate global alpha beta
	saedb::SynchronousEngine<AlphaBeta> alphaBetaEngine(graph);
	//aggregator
	AlphaBetaType* initAlphaBeta = new AlphaBetaType();
	globalAlphaBeta = new AlphaBetaAggregator();
	globalAlphaBeta->init(initAlphaBeta);
	alphaBetaEngine.registerAggregator("global_alpha_beta", globalAlphaBeta);
}
