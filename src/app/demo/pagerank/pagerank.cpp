#include <iostream>
#include <string>
#include <algorithm>

#include "sae_include.hpp"
#include "sample_data.hpp"


class FloatMaxAggregator: public saedb::IAggregator
{
public:
    void init(void* i){
        accu = *((float*)i);
    }
    
    void reduce(void* next){
        accu = std::max(accu, *((float*)next));
    }
    
    void* data() const{
        return (void*)(&accu);
    }
    
    ~FloatMaxAggregator() {}
    
private:
    float accu;
};


float RESET_PROB = 0.15;
float TOLERANCE = 1.0E-2;

typedef float vertex_data_type;
typedef float edge_data_type;
typedef saedb::empty message_date_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

class pagerank:
public saedb::IAlgorithm<graph_type, float>
{
public:
    void init(icontext_type& context, vertex_type& vertex) {
        vertex.data() = 1.0;
    }
    
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
        saedb::IAggregator* max_aggregator = context.getAggregator("max_pagerank");
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
    
    saedb::IEngine<pagerank> *engine = new saedb::EngineDelegate<pagerank>(graph);
    
    // aggregator
    float* init_rank = new float(0);
    saedb::IAggregator* max_pagerank = new FloatMaxAggregator();
    max_pagerank->init(init_rank);
    engine->registerAggregator("max_pagerank", max_pagerank);
    
    // start engine
    engine->signalAll();
    engine->start();
    
    std::cout << "max pagerank: " << *((float*)max_pagerank->data()) << std::endl;
    std::cout << "Done, do some cleaning......" << std::endl;
    delete engine;
    return 0;
}
