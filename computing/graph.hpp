#ifndef SAE_GRAPH_HPP
#define SAE_GRAPH_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <vector>

#include "storage/mgraph.hpp"
#include "serialization/serialization.hpp"

#include "graph_basic_types.hpp"

using namespace sae::serialization;

/*
 * Wrapper for memory mapped graph.
 * */
namespace saedb
{
    class sae_graph{
    public:
        typedef saedb::sae_graph           graph_type;
        typedef saedb::vertex_id_type      vertex_id_type;
        typedef saedb::edge_id_type        edge_id_type;

        struct vertex_type;
        typedef bool edge_list_type;
        class edge_type;

    private:
        sae::io::MappedGraph* graph;
        std::vector<vertex_type> vertices_;
        int *v_valid_;

    public:

        sae_graph(){

        };

        ~sae_graph() {
            graph->Close();
            delete graph;
            delete [] v_valid_;
        }

        void finalize() { /* nop */}

        size_t num_in_edges(const vertex_id_type vid) const {
            auto ei = graph->Vertices();
            ei->MoveTo(vid);
            return ei->InEdgeCount();
        }

        size_t num_out_edges(const vertex_id_type vid) const {
            auto ei = graph->Vertices();
            ei->MoveTo(vid);
            return ei->OutEdgeCount();
        }

        void load_mgraph(const std::string& graph_name) {
            std::cout << graph_name << std::endl;
            graph = sae::io::MappedGraph::Open(graph_name.c_str());

            vertices_.resize(num_vertices());
            v_valid_ = new int[num_vertices()];
            memset(v_valid_, 0, num_vertices() * sizeof(int));
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
            // TODO adjust to new API
            /*
            for (auto ei = graph->Edges(); ei->Alive();ei->Next()) {
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
            */
        }


        struct vertex_type {
            sae::io::VertexIteratorPtr vi;

            vertex_type(sae::io::VertexIteratorPtr&& vi) : vi(std::move(vi)) { 
            }

            vertex_type() { /*for saving into vector*/ }

            bool operator==(vertex_type& v) const {
                return vi->GlobalId() == v.vi->GlobalId();
            }

            template<typename T>
            T parse() {
                T ret = sae::serialization::convert_from_string<T>(vi->Data());
                return ret;
            }

            template<typename T>
            void update(T d) {
                vi->Data() = sae::serialization::convert_to_string<T>(d);
            }

            /* return the data rank of this vertex */
            std::string data_type_name() {
                return vi->TypeName();
            }

            uint32_t data_type_id() {
                return vi->TypeId();
            }

            size_t num_in_edges() const {
                return vi->InEdgeCount();
            }

            size_t num_out_edges() const {
                return vi->OutEdgeCount();
            }

            vertex_id_type id() const {
                return vi->GlobalId();
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

            edge_type() { /*for saving into vector*/ }

            vertex_type source() const {
                return vertex_type(std::move(ei->Source()));
            }

            vertex_type target() const {
                return vertex_type(std::move(ei->Target()));
            }

            template<typename T>
            T parse() {
                T ret = sae::serialization::convert_from_string<T>(ei->Data());
                return ret;
            }

            template<typename T>
            void update(T d) {
                ei->Data() = sae::serialization::convert_to_string<T>(d);
            }

            /* return the data rank of this vertex */
            std::string data_type_name() {
                return ei->TypeName();
            }

            uint32_t data_type_id() {
                return ei->TypeId();
            }

        };

        vertex_type& vertex(vertex_id_type vid){
            if (!v_valid_[vid]) {
                sae::io::VertexIteratorPtr v = graph->Vertices();
                v->MoveTo(vid);
                std::string s = v->Data();
                vertices_[vid] = vertex_type(std::move(v));
                v_valid_[vid] = 1;
            }
            return vertices_[vid];
        }
    };
}

#endif
