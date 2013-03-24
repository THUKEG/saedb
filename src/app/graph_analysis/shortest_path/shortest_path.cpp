#include <iostream>
#include "sae_include.hpp"
#include "sample_data.hpp"


typedef float edge_data_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;


class shortest_path:
    public saedb::sae_algorithm<graph, float>{
private:


public:



}


int main(){
    std::cout << "Shortest Path...\n";

    //load graph
    graph_type graph = sample_graph();
    std::cout << "#vertices: "
        << graph.num_vertices()
        << " #edges:"
        << graph.num_edges()
        << std::endl;

    //running the engine
    sae::sae_synchronous_engine<shortest_path> engine(graph);
    engine.start();

    std::cout << "Done" << std::endl;
    return 0;
}
