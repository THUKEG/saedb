#ifndef SAE_GRAPH_HPP
#define SAE_GRAPH_HPP

#include <string>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "io/mgraph.hpp"

#include "graph_basic_types.hpp"
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

        void load_mgraph(const std::string& graph_name) {
            std::cout << graph_name << std::endl;
            graph = sae::io::MappedGraph::Open(graph_name.c_str());
        }

        void load_format(const std::string& filename, const std::string& fmt = "mgraph") {
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

        template <  typename graph_builder_t,
                    typename filter_qeury_t>
        void filter(graph_builder_t& builder,
                    filter_qeury_t query){
            // slow implementation
            for (auto ei = graph->ForwardEdges(); ei->Alive();ei->Next()) {
                vertex_data_type* source = (vertex_data_type*)(ei->Source()->Data());
                vertex_data_type* target = (vertex_data_type*)(ei->Target()->Data());
                if (query.vertex_predicate(source) && query.vertex_predicate(target)) {
                    auto nsource = query.vertex_transform(source);
                    auto ntarget = query.vertex_transform(target);
                    auto nedge   = query.edge_transform((edge_data_type*)ei->Data());
                    builder.AddVertex(ei->SourceId(), nsource);
                    builder.AddVertex(ei->TargetId(), ntarget);
                    builder.AddEdge(ei->SourceId(), ei->TargetId(), nedge);
                }
            }
        }


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
