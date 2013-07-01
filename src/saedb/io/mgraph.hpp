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

        typedef unique_ptr<VertexIterator> VertexIteratorPtr;
        typedef unique_ptr<EdgeIterator> EdgeIteratorPtr;
        typedef unique_ptr<DataTypeAccessor> DataTypePtr;

        struct VertexIterator {
            virtual vid_t GlobalId() = 0;
            virtual void* Data() = 0;
            virtual void Next() = 0;
            virtual void NextOfType() = 0;
            virtual void MoveTo(vid_t) = 0;
            virtual bool Alive() = 0;
            virtual eid_t InEdgeCount() = 0;
            virtual eid_t OutEdgeCount() = 0;
            virtual EdgeIteratorPtr InEdges() = 0;
            virtual EdgeIteratorPtr OutEdges() = 0;
            virtual VertexIteratorPtr Clone() = 0;
            VertexIterator(){}
            virtual ~VertexIterator() {};
        };

        struct EdgeIterator {
            virtual eid_t GlobalId() = 0;
            virtual vid_t SourceId() = 0;
            virtual vid_t TargetId() = 0;
            virtual VertexIteratorPtr Source() = 0;
            virtual VertexIteratorPtr Target() = 0;
            virtual void* Data() = 0;
            virtual void Next() = 0;
            virtual bool Alive() = 0;
            virtual eid_t Count() = 0;
            virtual EdgeIteratorPtr Clone() = 0;
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
            virtual VertexIteratorPtr Vertices() = 0;

            virtual VertexIteratorPtr VerticesOfType(const char*) = 0;

            /**
             * Obtain an iterator for edges, sorted by sources.
             */
            virtual EdgeIteratorPtr ForwardEdges() = 0;

            /**
             * Obtain an iterator for edges, sorted by targets.
             */
            virtual EdgeIteratorPtr BackwardEdges() = 0;

            /*
                Obtain an iterator for edges.
            */
            virtual EdgeIteratorPtr Edges() = 0;

            /**
             * get data type accessor by name
             */
            virtual DataTypeAccessor* VertexDataType(const char * name) = 0;

            virtual DataTypeAccessor* EdgeDataType(const char* name) = 0;

            /**
             * Get all meta information about all data types
             */
            virtual std::vector<DataTypeAccessor*> VertexDataTypes() = 0;

            virtual std::vector<DataTypeAccessor*> EdgeDataTypes() = 0;

            /**
             * Force sync the mapped files with disk.
             *
             * Note that operating system will sync with disk even you have
             * never called this function.
             */
            virtual void Sync() = 0;

            /**
             * Close memory mapped files.
             */
            virtual void Close() = 0;

            /**
             * Default constructor. Do not use this directly.
             */
            MappedGraph() = default;

            /**
             * Default deconstructor.
             */
            virtual ~MappedGraph() = default;
        };

        struct MappedGraphWriter : public GraphWriter {
            static MappedGraphWriter* Open(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_type_count, uint32_t edge_data_type_count, uint32_t vertex_type_total_size, uint32_t edge_type_total_size, uint32_t* vertex_type_count, uint32_t* edge_type_count, uint32_t* vertex_data_size, uint32_t* edge_data_size);
        };
    }
}

