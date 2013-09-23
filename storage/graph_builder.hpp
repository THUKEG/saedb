#pragma once
#include <cstdint>
#include <cstddef>
#include <map>
#include <vector>
#include <utility>
#include <sstream>
#include <cstring>
#include <string>

#include "serialization/serialization.hpp"

namespace sae {
namespace io {

    typedef std::uint64_t vid_t;
    typedef std::uint64_t eid_t;
    using std::size_t;
    using std::map;
    using std::vector;
    using std::make_pair;

    enum GraphStorageType {
        MemoryMappedGraph = 0,
        GraphStorageTypeCount
    };

    struct edge_with_data {
        vid_t source;
        vid_t target;
        eid_t global_id;
        eid_t local_id;
        std::string data;
        uint32_t type_name;
        edge_with_data(vid_t source, vid_t target, eid_t global_id, eid_t local_id, std::string data, uint32_t type_name) :
            source(source), target(target), global_id(global_id), local_id(local_id), data(data), type_name(type_name) {}
    };

    struct vertex_with_data {
        vid_t global_id;
        vid_t local_id;
        uint32_t type_name;
        std::string data;
        vertex_with_data(vid_t global_id, vid_t local_id, uint32_t type_name, std::string data) :
            global_id(global_id), local_id(local_id), type_name(type_name), data(data) {
        }
    };

    struct data_type {
        uint32_t count;
        std::string type_name;
        data_type(std::string type_name): type_name(type_name) {
            count = 0;
        }
    };

    struct GraphWriter {
        virtual void AppendVertex(vid_t, vid_t, uint32_t, std::string) = 0;
        virtual void AppendEdge(vid_t, vid_t, eid_t, eid_t, uint32_t, std::string) = 0;
        virtual void AppendVertexDataType(std::string, uint32_t count) = 0;
        virtual void AppendEdgeDataType(std::string, uint32_t count) = 0;
        virtual void Close() = 0;
        virtual ~GraphWriter(){}
    };

    GraphWriter* CreateMemoryMappedGraphWriter(const char*, vid_t, eid_t, uint32_t, uint32_t, uint32_t*, uint32_t*);

    template<typename vkey_t>
    struct GraphBuilder
    {
        GraphBuilder() {
            AddVertexDataType("DefaultVertexType");
            AddEdgeDataType("DefaultEdgeType");
        }

        template <typename T>
        void AddVertex(vkey_t key, T data, const char* data_type_name = "DefaultVertexType") {
            int data_type_rank = -1;
            for (int i=0, size = vertex_data_types.size(); i<size; i++) {
                if (vertex_data_types[i].type_name == std::string(data_type_name)) {
                    data_type_rank = i;
                    break;
                }
            }
            if (data_type_rank == -1) {
                std::cerr << "ERROR: type " << data_type_name << " is undefined." << std::endl;
                return;
            }
            auto id = map(key);
            auto local_id = vertex_data_types[data_type_rank].count ++;

            std::string code = sae::serialization::convert_to_string(data);

            vertices[id] = vertex_with_data(id, local_id, data_type_rank, code);
        }

        template <typename T>
        void AddEdge(vkey_t source, vkey_t target, T data, const char* data_type_name = "DefaultEdgeType") {
            int data_type_rank = -1;
            for (int i=0, size = edge_data_types.size(); i<size; i++) {
                if (edge_data_types[i].type_name == std::string(data_type_name)) {
                    data_type_rank = i;
                    break;
                }
            }
            if (data_type_rank == -1) {
                std::cerr << "ERROR: type " << data_type_name << " is undefined." << std::endl;
                return;
            }

            if (vid_map.count(source) == 0) {
                std::cerr << "ERROR: adding edge <" << source << ", " << target << ">, source vertex not found! Edge not added." << std::endl;
                return;
            }
            if (vid_map.count(target) == 0) {
                std::cerr << "ERROR: adding edge <" << source << ", " << target << ">, target vertex not found! Edge not added." << std::endl;
                return;
            }

            auto sid = map(source);
            auto tid = map(target);

            eid_t id = edges.size();
            auto local_id = edge_data_types[data_type_rank].count ++;

            std::string code = sae::serialization::convert_to_string(data);

            edges.push_back(edge_with_data(sid, tid, id, local_id, code, data_type_rank));
        }

        virtual vid_t VertexCount() {
            return vertices.size();
        }

        virtual eid_t EdgeCount() {
            return edges.size();
        }

        virtual bool Save(const char * prefix, GraphStorageType type = MemoryMappedGraph) {
            if (type == MemoryMappedGraph) {
                uint32_t * vertex_type_count = new uint32_t[vertex_data_types.size()];
                for (int i=0, size = vertex_data_types.size(); i<size; i++) {
                    vertex_type_count[i] = vertex_data_types[i].count;
                    if (vertex_type_count[i] == 0) vertex_type_count[i] = 1;
                }

                uint32_t * edge_type_count = new uint32_t[edge_data_types.size()];
                for (int i=0, size = edge_data_types.size(); i<size; i++) {
                    edge_type_count[i] = edge_data_types[i].count;
                    if (edge_type_count[i] == 0) edge_type_count[i] = 1;
                }

                GraphWriter* writer = CreateMemoryMappedGraphWriter(prefix, vertices.size(), edges.size(), vertex_data_types.size(), edge_data_types.size(), vertex_type_count, edge_type_count);

                if (!writer) return false;
                for (auto data_type: vertex_data_types) {
                    writer->AppendVertexDataType(data_type.type_name, data_type.count);
                }
                for (auto data_type: edge_data_types) {
                    writer->AppendEdgeDataType(data_type.type_name, data_type.count);
                }
                for (auto& v : vertices) {
                    writer->AppendVertex(v.global_id, v.local_id, v.type_name, v.data);
                }
                for (auto& e: edges) {
                    writer->AppendEdge(e.source, e.target, e.global_id, e.local_id, e.type_name, e.data);
                }
                writer->Close();

                delete writer;
                delete [] vertex_type_count;
                delete [] edge_type_count;

                return true;
            }

            return false;
        }

        virtual void AddVertexDataType(const char* type_name) {
            vertex_data_types.push_back(data_type(std::string(type_name)));
        }

        virtual void AddEdgeDataType(const char* type_name) {
            edge_data_types.push_back(data_type(std::string(type_name)));
        }

        // no destructor?
    private:
        std::vector<vertex_with_data> vertices;
        std::vector<edge_with_data> edges;

        std::vector<data_type> vertex_data_types;
        std::vector<data_type> edge_data_types;

        std::map<vkey_t, vid_t> vid_map;

        vid_t map(vkey_t key) {
            vid_t result;
            auto it = vid_map.find(key);
            if (it == vid_map.end()) {
                result = vertices.size();
                vertices.push_back(vertex_with_data(0, 0, 0, std::string("")));
                vid_map.insert(make_pair(key, result));
            } else {
                result = it->second;
            }

            return result;
        }
    };

}
}
