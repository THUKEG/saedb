#include <iostream>
#include <string>
#include <math.h>

#include "sae_include.hpp"
#include "sample_data.hpp"


class Float_max_aggregator: public saedb::aggregator
{
public:
    void init(void* i){
        accu = *((float*)i);
    }
    
    void reduce(void* next){
        accu = max(accu, *((float*)next));
    }
    
    void* data() const{
        return (void*)(&accu);
    }
    
private:
    float accu;
};


float RESET_PROB = 0.15;
float TOLERANCE = 1.0E-2;

typedef float vertex_data_type;
typedef float edge_data_type;
typedef saedb::empty message_date_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

class pagerank :
public saedb::sae_algorithm<graph_type, float>
{
public:
    
    edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const{
	    return saedb::IN_EDGES;
    }
    
    float gather(icontext_type& context, const vertex_type& vertex,
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
    
    void aggregate(icontext_type& context, const vertex_type& vertex){
        saedb::aggregator* max_aggregator = context.GetAggregator("max_pagerank");
        float t = vertex.data();
        max_aggregator->reduce(&t);
    }
};

graph_type sample_graph(){
    return LOAD_SAMPLE_GRAPH<graph_type>();
}

int main(){
    std::string graph_dir;
    std::string format = "adj";
    
    // todo read graph_dir and format
    
    graph_type graph = sample_graph();
    // graph.load_format(graph_dir, format);
    
    std::cout << "#vertices: "
    << graph.num_vertices()
    << " #edges:"
    << graph.num_edges() << std::endl;
    
    saedb::sae_synchronous_engine<pagerank> engine(graph);
    
    // aggregator
    float* init_rank = new float(0);
    Float_max_aggregator* max_pagerank = new Float_max_aggregator();
    max_pagerank->init(init_rank);
    engine.RegisterAggregator("max_pagerank", max_pagerank);
    
    // start engine
    engine.signal_all();
    engine.start();
    
    std::cout << "Done" << std::endl;
    return 0;
}
