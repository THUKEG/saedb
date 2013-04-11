#include <iostream>
#include <string>
#include <algorithm>

#include "sae_include.hpp"

double RESET_PROB = 0.15;
double TOLERANCE = 1.0E-2;

typedef double vertex_data_type;
typedef double edge_data_type;
typedef saedb::empty message_date_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;




class pagerank:
public saedb::IAlgorithm<graph_type, double>
{
public:
    void init(icontext_type& context, vertex_type& vertex) {
        vertex.data() = 0.2;
    }

    edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    double gather(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
        return ((1.0 - RESET_PROB) / edge.source().num_out_edges()) *
        edge.source().data();
    }

    void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total){
        const double newval = total + RESET_PROB;
        vertex.data() = newval;
    }

    edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
        return saedb::OUT_EDGES;
    }

    void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
        context.signal(edge.target());
    }

};


struct float_max{
	float value;

	float_max(): value(0){ }
	float_max(float value){
		this->value = value;
	}

	float_max& operator+=(const float_max& other){
		this->value = std::max(this->value, other.value);
		return *this;
	}
};

float_max floatMaxAggregator(const graph_type::vertex_type& vertex){
	return float_max(vertex.data());
}


float_max max_pagerank;

int main(){
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

    saedb::IEngine<pagerank> *engine = new saedb::EngineDelegate<pagerank>(graph);
    // start engine
    engine->signalAll();
    engine->start();

    max_pagerank = engine->map_reduce_vertices<float_max>(floatMaxAggregator);


    std::cout << "max pagerank: " << max_pagerank.value << std::endl;
    std::cout << "Done, do some cleaning......" << std::endl;
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        std::cout << "v[" << i << "]: " << graph.vertex(i).data() << endl;
    }

    delete engine;
    return 0;
}
