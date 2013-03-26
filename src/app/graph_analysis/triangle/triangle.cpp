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

class vdata{
public:


}

//define the triangle count data structure
class triangle_count{
public:
    int out_triangles;
    int in_triangles;
    int through_triangles;
    int cycle_triangles;

    triangle_count& operator+=(const triangle_count& other){
        out_triangles += other.out_triangles;
        in_triangles += other.in_triangles;
        through_triangles += other.through_triangles;
        cycle_triangles += other.cycle_triangles;
        return *this;
    }
}

/**
 * @brief Triangle Counting
 * This implemnts the counting procedure decribed in
 *  Efficient Algorithms for Large-Scale Local Triangle Counting
 *
 */
class triangle:
    public saedb::sae_algorithm<graph, float>{
private:
    bool perform_scatter;

public:
    //gather on all edges
    edge_dir_type gather_edge(icontext_type& context,
            const vertex_type& vertex) const{
        return saedb::ALL_EDGES;
    }

    gather_type gather(icontext_type& context,
            const vertex_type& vertex,
            edge_type& edge) const{

    }

    void apply(icontext_type& context,
            vertex_type& vertex,
            const gather_type& total){

    }

    edge_dir_type scatter_edge(icontext_type& context,
            const vertex_type& vertex) const{
        return
    }

    void scatter(icontext_type& context,
            const vertex_type& vertex,
            edge_type& edge) const{

    }
}


int main(){
    std::cout << "Triangle Counting...\n";

    //load graph
    graph_type graph = sample_graph();
    std::cout << "#vertices: "
        << graph.num_vertices()
        << " #edges:"
        << graph.num_edges()
        << std::endl;

    //running the engine
    saedb::sae_synchronous_engine<triangle> engine(graph);
    engine.start();

    std::cout << "Done" << std::endl;
    return 0;
}
