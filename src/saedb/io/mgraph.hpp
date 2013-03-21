#pragma once

/*
 * Files in a Graph:
 * Meta
 * Forward Edge List
 * Backward Edge List
 * Vertices Data
 * Edge Data
 */

#include <cstdint>

namespace sae {
    namespace io {
        typedef uint64_t vid_t;
        typedef uint64_t eid_t;

        struct VertexIterator {
            virtual vid_t Id() = 0;
            virtual void* Data() = 0;
            virtual void Next() = 0;
            virtual bool Alive() = 0;
            virtual EdgeIterator* InEdges() = 0;
            virtual EdgeIterator* OutEdges() = 0;
            VertexIterator(){}
            virtual ~VertexIterator() {};
        };

        struct EdgeIterator {
            virtual eid_t Id() = 0;
            virtual vid_t Source() = 0;
            virtual vid_t Target() = 0;
            virtual vid_t* MutableSource() = 0;
            virtual vid_t* MutableTarget() = 0;
            virtual vid_t* MutableDocId() = 0;
            virtual void* Data() = 0;
            virtual void Next() = 0;
            virtual bool Alive() = 0;
            EdgeIterator(){}
            virtual ~EdgeIterator() {};
        };

        struct MappedGraph {
            static MappedGraph* Open(const char * prefix);
            static MappedGraph* Create(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_size, uint32_t edge_data_size);

            virtual vid_t VertexCount() = 0;
            virtual eid_t EdgeCount() = 0;

            virtual VertexIterator* Vertices() = 0;
            virtual EdgeIterator* Edges() = 0;
            virtual void Optimize() = 0;
            virtual void Close() = 0;
            MappedGraph(){}
            virtual ~MappedGraph() {};
        };
    }
}

