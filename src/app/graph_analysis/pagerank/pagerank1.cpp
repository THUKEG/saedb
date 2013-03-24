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
#include "sae_include.hpp"
#include "sample_data.cpp"

//define data type
typedef float vertex_data_type;
typedef float edge_data_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

//define constants
#define RESET_PROB 0.15

//graph data
graph_type sample_graph(){
    return LOAD_SAMPLE_GRAPH<graph_type>();
}

/**
 * @brief PageRank
 */
class pagerank:
    public sae::sae_algorithm<graph_type, float>{
private:

public:
    //gather on all in edges
    edge_dir_type gather_edges(icontext_type& context,
            const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    vertex_data_type gather(icontext_type& context,
            const vertex_type& vertex,
            edge_type& edge) const{
        //gather the current pagerank value on source of all the in-edges and divide by it's out-degree
        return edge.source().data() / edge.source().num_out_edges();
    }//all data gathered will be sum implicitly, overload operator for vertex_type to customize the sum function

    void apply(icontext_type& context,
            vertex_type& vertex,
            const gather_type& total){
        //all data gathered had been summed into total
        //the new pagerank will be pass into vertex data
        double val = total*(1-RESET_PROB) + RESET_PROB;
        vertex.data() = val;
        //schedule the program by a iteration number

    }

    //no need for scatter, but my use scatter as a scheduler to signal the next iteration
    edge_dir_type scatter_edges(icontext_type& context,
            const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }

}



int main(){
    std::cout << "PageRank..." << std::endl;

    //load graph
    graph_type graph = sample_graph();
    std::cout << "#vertices: "
        << graph.num_vertices()
        << " #edges:"
        << graph.num_vertices()
        << std::endl;

    //running the engine
    saedb::sae_synchronous_engine<pagerank> engine(graph);
    engine.start();

    std::cout << "Done" << std::endl;
    return 0;
}
