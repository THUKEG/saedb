#ifndef SAE_GRAPH_HPP
#define SAE_GRAPH_HPP

#include <string>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "io/mgraph.hpp"

#include "graph_basic_types.hpp"
using namespace std;
/*
 * Wrapper for memory mapped graph.
 * */
namespace saedb
{
    template<typename vertex_data_t,
             typename edge_data_t>
    class sae_graph
    {
    public:
        typedef vertex_data_t                                   vertex_data_type;
        typedef edge_data_t                                     edge_data_type;
        typedef saedb::sae_graph<vertex_data_t, edge_data_t>    graph_type;
        typedef saedb::vertex_id_type                           vertex_id_type;
        typedef saedb::edge_id_type                             edge_id_type;

        struct vertex_type;
        typedef bool edge_list_type;
        class edge_type;

    private:
        sae::io::MappedGraph* graph;

    public:

        sae_graph(){

        };

        ~sae_graph() {
            graph->Close();
            delete graph;
        }

        void finalize() { /* nop */}

        size_t num_in_edges(const vertex_id_type vid) const {
            auto ei = graph->Vertices();
            ei->MoveTo(vid);
            return ei->InEdges()->Count();
        }

        size_t num_out_edges(const vertex_id_type vid) const {
            auto ei = graph->Vertices();
            ei->MoveTo(vid);
            return ei->OutEdges()->Count();
        }

        void load_mgraph(const string& graph_name) {
            cout << graph_name << endl;
            graph = sae::io::MappedGraph::Open(graph_name.c_str());
        }

        void load_format(const string& filename, const string& fmt = "mgraph") {
            if (fmt == "mgraph")
                load_mgraph(filename);
        }

        void save() const {
            graph->Sync();
        }

        void display() {
        }

        size_t num_vertices() {
            return graph->VertexCount();
        }

        size_t num_edges() {
            return graph->EdgeCount();
        }

        size_t num_local_vertices () {
            return num_vertices();
        }

        /**
         * \brief Performs a map-reduce operation on each vertex in the
         * graph returning the result.
         *
         * Given a map function, map_reduce_vertices() call the map function on all
         * vertices in the graph. The return values are then summed together and the
         * final result returned. The map function should only read the vertex data
         * and should not make any modifications. map_reduce_vertices() must be
         * called on all machines simultaneously.
         *
         * ### Basic Usage
         * For instance, if the graph has float vertex data, and float edge data:
         * \code
         *   typedef graphlab::distributed_graph<float, float> graph_type;
         * \endcode
         *
         * To compute an absolute sum over all the vertex data, we would write
         * a function which reads in each a vertex, and returns the absolute
         * value of the data on the vertex.
         * \code
         * float absolute_vertex_data(const graph_type::vertex_type& vertex) {
         *   return std::fabs(vertex.data());
         * }
         * \endcode
         * After which calling:
         * \code
         * float sum = graph.map_reduce_vertices<float>(absolute_vertex_data);
         * \endcode
         * will call the <code>absolute_vertex_data()</code> function
         * on each vertex in the graph. <code>absolute_vertex_data()</code>
         * reads the value of the vertex and returns the absolute result.
         * This return values are then summed together and returned.
         * All machines see the same result.
         *
         * The template argument <code><float></code> is needed to inform
         * the compiler regarding the return type of the mapfunction.
         *
         * The optional argument vset can be used to restrict he set of vertices
         * map-reduced over.
         *
         * ### Relations
         * This function is similar to
         * graphlab::iengine::map_reduce_vertices()
         * with the difference that this does not take a context
         * and thus cannot influence engine signalling.
         * transform_vertices() can be used to perform a similar
         * but may also make modifications to graph data.
         *
         * \tparam ReductionType The output of the map function. Must have
         *                    operator+= defined, and must be \ref sec_serializable.
         * \tparam VertexMapperType The type of the map function.
         *                          Not generally needed.
         *                          Can be inferred by the compiler.
         * \param mapfunction The map function to use. Must take
         *                   a \ref vertex_type, or a reference to a
         *                   \ref vertex_type as its only argument.
         *                   Returns a ReductionType which must be summable
         *                   and \ref sec_serializable .
         * \param vset The set of vertices to map reduce over. Optional. Defaults to
         *             complete_set()
         */
//         template <typename ReductionType, typename MapFunctionType>
//         ReductionType map_reduce_vertices(MapFunctionType mapfunction){
//                                          // const vertex_set& vset = complete_set()) {

//
//         } // end of map_reduce_vertices

        struct vertex_type {
            sae::io::VertexIteratorPtr vi;

            vertex_type(sae::io::VertexIteratorPtr&& vi) : vi(std::move(vi)) { }

            bool operator==(vertex_type& v) const {
                return vi->Id() == v->vi->Id();
            }

            const vertex_data_type& data() const {
                return *((vertex_data_type *) vi->Data());
            }

            vertex_data_type& data() {
                return *((vertex_data_type *) vi->Data());
            }

            size_t num_in_edges() const {
                return vi->InEdgeCount();
            }

            size_t num_out_edges() const {
                return vi->OutEdgeCount();
            }

            vertex_id_type id() const {
                return vi->Id();
            }

            sae::io::EdgeIteratorPtr in_edges() {
                return vi->InEdges();
            }

            sae::io::EdgeIteratorPtr out_edges() {
                return vi->OutEdges();
            }

            lvid_type local_id() const{
                return id();
            }
        };

        class edge_type {
        private:
            sae::io::EdgeIteratorPtr ei;
        public:

            edge_type(sae::io::EdgeIteratorPtr&& ei) : ei(std::move(ei)) { }

            vertex_type source() const {
                return vertex_type(std::move(ei->Source()));
            }

            vertex_type target() const {
                return vertex_type(std::move(ei->Target()));
            }

            const edge_data_type& data() const {
                return *(edge_data_type*) ei->Data();
            }

            edge_data_type& data() {
                return *(edge_data_type*) ei->Data();
            }
        };

        vertex_type vertex(vertex_id_type vid){
            sae::io::VertexIteratorPtr v = graph->Vertices();
            v->MoveTo(vid);
            return vertex_type(std::move(v));
        }
    };
}

#endif
