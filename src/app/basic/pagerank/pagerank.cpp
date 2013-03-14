#include <iostream>
#include <string>
#include "sae_include.hpp"

typedef float vertex_data_type;
typedef float edge_data_type;
typedef saedb::empty message_data_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

class pagerank: public saedb::sae_algorithm<graph_type, float>{
public:
    //gather on all in-edges
    edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const{
        return graphlab::INEDGES;
    }

    //gather the weighted sum of the edge
    float gather(icontext_type& context, const vertex_type& vertex, edge_type& edge){


        return 0;
    }


    void apply(icontext_type& context, vertex_type& vertex, const gather_type& total){


    }

    edge_dir_type scatter_edges(icontext_type& context, const vertex){

    }

    void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {

    }
}

graph_type sample_graph(){

}


int main(){
    std::string graph_dir;
    std::string format = "adj";

    graph_type graph = sample_graph();

    saedb::sae_synchronous_engine<pagerank> engine(graph);
    engine.start();
    return 0;
}
