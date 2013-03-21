#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mmap_helper.hpp"
#include "mgraph.hpp"

namespace sae {
namespace io {

using std::uint32_t;
using std::uint64_t;

struct GraphHeader {
    vid_t vertices;
    eid_t edges;
    uint32_t vertex_data_size;
    uint32_t edge_data_size;
};

struct EdgeListItem {
    vid_t source;
    vid_t target;
    eid_t docid;
};

struct VertexIteratorImpl : public VertexIterator {
    char* data;
    uint32_t data_size;
    vid_t index, count;

    VertexIteratorImpl(vid_t count, void* data, uint32_t data_size) {
        this->data = (char*) data;
        this->data_size = data_size;
        this->index = 0;
        this->count = count;
    }

    vid_t Id() {
        return index;
    }

    void* Data() {
        return data + data_size * index;
    }

    void Next() {
        index ++;
    }

    bool Alive() {
        return index < count;
    }
};

struct EdgeIteratorImpl : public EdgeIterator {
    EdgeListItem* list;
    char* data;
    uint32_t data_size;
    eid_t index, count;

    EdgeIteratorImpl(eid_t count, EdgeListItem* list, void* data, uint32_t data_size) {
        this->list = list;
        this->data = (char*) data;
        this->data_size = data_size;
        this->index = 0;
        this->count = count;
    }

    eid_t Id() {
        return index;
    }

    vid_t Source() {
        return list[index].source;
    }

    vid_t Target() {
        return list[index].target;
    }

    eid_t DocId() {
        return list[index].docid;
    }

    vid_t* MutableSource() {
        return &list[index].source;
    }

    vid_t* MutableTarget() {
        return &list[index].target;
    }

    eid_t* MutableDocId() {
        return &list[index].docid;
    }

    void* Data() {
        return data + data_size * list[index].docid;
    }

    void Next() {
        index ++;
    }

    bool Alive() {
        return index < count;
    }
};

namespace {

    char* concat(const char* a, const char* b) {
        static char buf[1024];
        sprintf(buf, "%s%s", a, b);
        return buf;
    }

    int source_comparer(const void * a, const void * b) {
        EdgeListItem* ea = (EdgeListItem*) a;
        EdgeListItem* eb = (EdgeListItem*) b;
        return ea->source - eb->source;
    }

    int target_comparer(const void * a, const void * b) {
        EdgeListItem* ea = (EdgeListItem*) a;
        EdgeListItem* eb = (EdgeListItem*) b;
        return ea->target - eb->target;
    }
}

struct MappedGraphImpl : public MappedGraph {
    MMapFile *meta_file, *forward_file, *backward_file, *vdata_file, *edata_file;
    GraphHeader* meta;
    EdgeListItem* forward;
    EdgeListItem* backward;
    void* vertexData;
    void* edgeData;

    static MappedGraphImpl* Open(const char * prefix) {
        MappedGraphImpl* mg = new MappedGraphImpl();

        mg->meta_file = MMapFile::Open(concat(prefix, ".meta"));
        mg->forward_file = MMapFile::Open(concat(prefix, ".forward"));
        mg->backward_file = MMapFile::Open(concat(prefix, ".backward"));
        mg->vdata_file = MMapFile::Open(concat(prefix, ".vdata"));
        mg->edata_file = MMapFile::Open(concat(prefix, ".edata"));

        mg->meta = (GraphHeader*) mg->meta_file->Data();
        mg->forward = (EdgeListItem*) mg->forward_file->Data();
        mg->backward = (EdgeListItem*) mg->backward_file->Data();
        mg->vertexData = mg->vdata_file->Data();
        mg->edgeData = mg->edata_file->Data();

        return mg;
    }

    static MappedGraphImpl* Create(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_size, uint32_t edge_data_size) {
        MappedGraphImpl* mg = new MappedGraphImpl();

        mg->meta_file = MMapFile::Create(concat(prefix, ".meta"), sizeof(GraphHeader));
        mg->forward_file = MMapFile::Create(concat(prefix, ".forward"), sizeof(EdgeListItem) * m);
        mg->backward_file = MMapFile::Create(concat(prefix, ".backward"), sizeof(EdgeListItem) * m);
        mg->vdata_file = MMapFile::Create(concat(prefix, ".vdata"), vertex_data_size * n);
        mg->edata_file = MMapFile::Create(concat(prefix, ".edata"), edge_data_size * m);

        mg->meta = (GraphHeader*) mg->meta_file->Data();
        mg->forward = (EdgeListItem*) mg->forward_file->Data();
        mg->backward = (EdgeListItem*) mg->backward_file->Data();
        mg->vertexData = mg->vdata_file->Data();
        mg->edgeData = mg->edata_file->Data();

        mg->meta->vertices = n;
        mg->meta->edges = m;
        mg->meta->vertex_data_size = vertex_data_size;
        mg->meta->edge_data_size = edge_data_size;

        return mg;
    }

    vid_t VertexCount() {
        return meta->vertices;
    }

    eid_t EdgeCount() {
        return meta->edges;
    }

    VertexIterator* Vertices() {
        return new VertexIteratorImpl(meta->vertices, vertexData, meta->vertex_data_size);
    }

    EdgeIterator* Edges() {
        return new EdgeIteratorImpl(meta->edges, forward, edgeData, meta->edge_data_size);
    }

    void Optimize() {
        // sort forward and backword edge lists
        qsort(forward, meta->edges, sizeof(EdgeListItem), source_comparer);
        qsort(backward, meta->edges, sizeof(EdgeListItem), target_comparer);
    }

    void Close() {
        if (meta_file) meta_file->Close();
        if (forward_file) forward_file->Close();
        if (backward_file) backward_file->Close();
        if (vdata_file) vdata_file->Close();
        if (edata_file) edata_file->Close();
    }
};

MappedGraph* MappedGraph::Open(const char * prefix) {
    return MappedGraphImpl::Open(prefix);
}

MappedGraph* MappedGraph::Create(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_size, uint32_t edge_data_size) {
    return MappedGraphImpl::Create(prefix, n, m, vertex_data_size, edge_data_size);
}

} // io
} // sae
