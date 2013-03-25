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

//define vertex data structure
class vdata{
public:
    long label;
    //the sum function finds the minimum id of the connected component
    vdata& operator+=(const vdata& other){
        label = std::min<long>(label, other.label);
        return *this;
    }
}

//define data type
typedef vdata vertex_data_type;
typedef float edge_data_type;
typedef saedb::empty message_date_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

//set label to vertex id
void initialize_vertex(graph_type::vertex_type& v){
    v.data().label = v.id();
    //reload the operator + to
    //sum perform founding the minimum value
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
    // set perform_scatter to determine which edges to scatter on
    bool perform_scatter;

public:
    edge_dir_type gather_edges(icontext_type& context,
            const vertex_type& vertex) const{
        return sae::ALL_EDGES;
    }

    vdata gather(incontex_type& context,
            const vertex_type& vertex,
            edge_type& edge){
        if(edge.source().id() != vertex.id()){
            return edge.source().data().label;
        }
        if(edge.target().id() != vertex.id()){
            return edge.target().data().label;
        }
    }

    //change the vertex data if any of its neighbors have a lower data value
    void apply(icontext_type& context,
            vertex_type& vertex,
            const gather_type& total){
        if(total < vertex.data().label){
            perform_scatter = true;
        }else{
            perform_scatter = false;
        }
    }

    edge_dir_type scatter_edges(icontext_type& context,
            const vertex_type& vertex) const{
        if(perform_scatter){
            return saedb::ALL_EDGES;
        }else{
            return saedb::NO_EDGES;
        }
    }

    void scatter(icontext_type& context,
            const vertex_type& vertex,
            edge_type& edge) const{
        //if a neighbor vertex has a bigger label, send a massage
        if(edge.source().id() != vertex.id()
                && edge.source().data().label > vertex.data().label){
            context.signal(edge.source());
        }
        if(edge.target().id() != vertex.id()
                && edge.target().data().label > vertex.data().label){
            context.signal(edge.target());
        }
    }
}

//require a reduce phase to aggregate all the data

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
