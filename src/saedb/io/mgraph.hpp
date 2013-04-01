#pragma once

/*
 * Files in a Graph:
 * Meta
 * Forward Edge List
 * Backward Edge List
 * Forward Edge Index
 * Backward Edge Index
 * Vertices Data
 * Edge Data
 */

#include <cstdint>
#include <memory>
#include "graph_builder.hpp"

namespace sae {
    namespace io {
        using std::unique_ptr;

        typedef uint64_t vid_t;
        typedef uint64_t eid_t;

        struct VertexIterator;
        struct EdgeIterator;

        struct VertexIterator {
            virtual vid_t Id() = 0;
            virtual void* Data() = 0;
            virtual void Next() = 0;
            virtual void MoveTo(vid_t) = 0;
            virtual bool Alive() = 0;
            virtual vid_t Count() = 0;
            virtual unique_ptr<EdgeIterator> InEdges() = 0;
            virtual unique_ptr<EdgeIterator> OutEdges() = 0;
            VertexIterator(){}
            virtual ~VertexIterator() {};
        };

        struct EdgeIterator {
            virtual eid_t Id() = 0;
            virtual vid_t SourceId() = 0;
            virtual vid_t TargetId() = 0;
            virtual unique_ptr<VertexIterator> Source() = 0;
            virtual unique_ptr<VertexIterator> Target() = 0;
            virtual void* Data() = 0;
            virtual void Next() = 0;
            virtual void MoveTo(eid_t) = 0;
            virtual bool Alive() = 0;
            virtual eid_t Count() = 0;
            EdgeIterator(){}
            virtual ~EdgeIterator() {};
        };

        struct MappedGraph {

            /**
             * Open an immutable for access.
             */
            static MappedGraph* Open(const char * prefix);

            /**
             * Count of vertices.
             */
            virtual vid_t VertexCount() = 0;

            /**
             * Count of edges.
             */
            virtual eid_t EdgeCount() = 0;

            /**
             * Obtain an iterator for vertices.
             */
            virtual unique_ptr<VertexIterator> Vertices() = 0;

            /**
             * Obtain an iterator for edges, sorted by sources.
             */
            virtual unique_ptr<EdgeIterator> ForwardEdges() = 0;

            /**
             * Obtain an iterator for edges, sorted by targets.
             */
            virtual unique_ptr<EdgeIterator> BackwardEdges() = 0;

            /**
             * Close memory mapped files.
             */
            virtual void Close() = 0;

            MappedGraph(){}
            virtual ~MappedGraph() {};
        };

        struct MappedGraphWriter : public GraphWriter {
            static MappedGraphWriter* Open(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_size, uint32_t edge_data_size);
        };
    }
}

