#ifndef SAE_GRAPH_HPP
#define SAE_GRAPH_HPP

#include <string>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "io/mgraph.hpp"
#include "io/graph_builder.hpp"

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
        sae::io::GraphBuilder<int, vertex_data_type, edge_data_type> builder;
        sae::io::MappedGraph* graph;

    public:

        sae_graph(){

        };

        void finalize() { /* nop */}

        size_t num_in_edges(const vertex_id_type vid) const {

        }

        size_t num_out_edges(const vertex_id_type vid) const {

        }

        void load(const string& graph_name) {

        }

        void loadformat ( string filename) {
        }

        void save() const {
        }

        void display(){
        }

        void add_vertex(vertex_id_type id, vertex_data_type data) {
        }

        void add_edge(vertex_id_type source, vertex_id_type target, edge_data_type data) {
        }

        size_t num_vertices() {
            return 0;
        }

        size_t num_edges() {
            return 0;
        }

        size_t num_local_vertices () {
            return num_vertices();
        }

        struct vertex_type {
            sae_graph& graph_ref;
            lvid_type lvid;

            vertex_type(sae_graph& graph_ref, lvid_type lvid):
            graph_ref(graph_ref), lvid(lvid) { }

            vertex_type() {}

            bool operator==(vertex_type& v) const {
                return lvid == v.lvid;
            }

            const vertex_data_type& data() const {
            }

            vertex_data_type& data() {
            }

            size_t num_in_edges() const {
                auto in_edge = graph_ref.vertex_id_2_in_edges[lvid];
                return in_edge.size();
            }

            size_t num_out_edges() const {
            }

            vertex_id_type id() const {
                return lvid;
            }

            vector<edge_type> in_edges() {
            }

            vector<edge_type> out_edges() {
            }

            lvid_type local_id() const{

            }
        };

        class edge_type {
        private:
            graph_type& graph_ref;

            vertex_id_type source_id;

            vertex_id_type target_id;

            friend class sae_graph;
        public:

            edge_type(sae_graph& graph, vertex_id_type source, vertex_id_type target):
            graph_ref(graph), source_id(source), target_id(target){}

            vertex_type source() const {
                return vertex_type(graph_ref, source_id);
            }

            vertex_type target() const {
                return vertex_type(graph_ref, target_id);
            }

            const edge_data_type& data() const {  }

            edge_data_type& data() { }
        };

        vertex_type vertex(vertex_id_type vid){
            return vertex_type(*this, vid);
        }
    };
}

#endif
