/* Copyright (C)
 * 2013 - Yutao Zhang
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
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
 * Output: find all connected components in the graph, and also count the number of the vertex of each connected components
 * Reference: http://en.wikipedia.org/wiki/Tarjan's_strongly_connected_components_algorithm
 */
class connected_component:
    public saedb::sae_algorithm<graph, float>{
private:
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
    std::cout << "Connected Component...\n";

    // load graph
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

    // running the engine
    saedb::sae_synchronous_engine<connected_component> engine(graph);
    // engine.signal_all();
    engine.start();

    std::cout << "Done" << std::endl;
    return 0;
}
