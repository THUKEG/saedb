#include <iostream>
#include <string>
#include "sae_include.hpp"
#include "sample_data.cpp"

typedef float vertex_data_type;
typedef float edge_data_type;
typedef saedb::empty message_date_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

void init_vertex(vertex_data_type& vertex){
    vertex.data() = -1;
}


/**
 * @brief Connected Component
 * Trajan's strong Connected Components algorithm
 * Input: a directed graph
 *
 * * Reference: http://en.wikipedia.org/wiki/Tarjan's_strongly_connected_components_algorithm
 */
class connected_component:
    public saedb::sae_algorithm<graph, float>
{
    // set changed to determine which edges to scatter on
    bool changed;
    // local copy of the message value
    vertex_data_type message_value;

public:

    void init(icontext_type& context,
            const vertex_type& vertex,
            const message_type& message){
        // message.value == * on first run, so init message_value to vertex data.
        if(message.value == 4294967295){
            message.value = vertex.id();
        }else{
            // else, set the local copy to the message parameter
            message_value = message.value;
        }
    }

    edge_dir_type gather_edges(icontext_type& context,
            const vertex_type& vertex) const{
        return sae::NO_EDGES;
    }

    float gather(icontext_type& context,
            const vertex_type& vertex,
            edge_type& edge) const{
        return 0;
    }

    // Change the vertex data if any of its neighbors have a lower data value
    void apply(icontext_type& context,
            vertex_type& vertex,
            const gather_type& total){
        if(message_value < vertex.)
    }

    edge_dir_type scatter_edges(icontext_type& context,
            const vertex_type& vertex) const{
        if(changed){
            return sae::ALL_EDGES;
        }else{
            return sae::NO_EDGES;
        }
    }

    void scatter(icontext_type& context,
            const vertex_type& vertex,
            edge_type& edge) const{

    }
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
    // graph.transform_vertices(init_vertex);
    saedb::sae_synchronous_engine<connected_component> engine(graph);
    // engine.signal_all();
    engine.start();
    std::cout << "Done" << std::endl;
    return 0;
}
