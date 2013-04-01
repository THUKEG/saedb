#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory>

#include "mmap_file.hpp"
#include "mgraph.hpp"

namespace sae {
namespace io {

using std::uint32_t;
using std::uint64_t;
using std::unique_ptr;


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


namespace {

    const char * concat(const char * a, const char * b) {
        static char buf[1024];
        snprintf(buf, sizeof(buf), "%s%s", a, b);
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


struct GraphData {
    GraphHeader* meta;
    vid_t *findex, *bindex;
    EdgeListItem *forward, *backward;
    char *vertexData;
    char *edgeData;
};


// Evil forward declartion.
unique_ptr<EdgeIterator> CreateEdgeIterator(const GraphData& g, eid_t base, eid_t count, EdgeListItem* list);


struct VertexIteratorImpl : public VertexIterator {
    const GraphData& g;
    char* data;
    uint32_t data_size;
    vid_t base, index, count;

    VertexIteratorImpl(const GraphData& g, vid_t base, vid_t count) :
        g(g), base(base), index(0), count(count)
    {
        this->data = (char*) g.vertexData;
        this->data_size = g.meta->vertex_data_size;
    }

    vid_t Id() {
        return base + index;
    }

    void* Data() {
        return data + data_size * (base + index);
    }

    void Next() {
        index ++;
    }

    void MoveTo(vid_t id) {
        index = id;
    }

    unique_ptr<EdgeIterator> InEdges() {
        vid_t id = Id();
        return CreateEdgeIterator(g, g.bindex[id], g.bindex[id + 1] - g.bindex[id], g.backward);
    }

    unique_ptr<EdgeIterator> OutEdges() {
        vid_t id = Id();
        return CreateEdgeIterator(g, g.findex[id], g.findex[id + 1] - g.findex[id], g.forward);
    }

    bool Alive() {
        return index < count;
    }

    vid_t Count() {
        return count;
    }
};


struct EdgeIteratorImpl : public EdgeIterator {
    const GraphData& g;
    vid_t* edge_index;
    EdgeListItem *list;
    char *data;
    uint32_t data_size;
    eid_t base, index, count;

    EdgeIteratorImpl(const GraphData& g, eid_t base, eid_t count, EdgeListItem* list) :
        g(g), base(base), count(count), index(0), list(list)
    {
        this->data = (char*) g.edgeData;
        this->data_size = g.meta->edge_data_size;
    }

    eid_t Id() {
        return base + index;
    }

    vid_t SourceId() {
        return list[Id()].source;
    }

    vid_t TargetId() {
        return list[Id()].target;
    }

    unique_ptr<VertexIterator> Source() {
        vid_t source = list[Id()].source;
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, source, 1));
    }

    unique_ptr<VertexIterator> Target() {
        vid_t target = list[Id()].target;
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, target, 1));
    }

    eid_t DocId() {
        return list[Id()].docid;
    }

    void* Data() {
        return data + data_size * list[Id()].docid;
    }

    void Next() {
        index ++;
    }

    void MoveTo(eid_t id) {
        index = id;
    }

    bool Alive() {
        return index < count;
    }

    eid_t Count() {
        return count;
    }
};


unique_ptr<EdgeIterator> CreateEdgeIterator(const GraphData& g, eid_t base, eid_t count, EdgeListItem* list) {
    return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, base, count, list));
}


struct MappedGraphImpl : public MappedGraph {
    unique_ptr<MMapFile> meta_file, forward_index_file, forward_file, backward_index_file, backward_file, vdata_file, edata_file;
    GraphData g;

    static MappedGraphImpl* Open(const char * prefix) {
        MappedGraphImpl* mg = new MappedGraphImpl;

        mg->meta_file.reset(MMapFile::Open(concat(prefix, ".meta")));
        mg->forward_index_file.reset(MMapFile::Open(concat(prefix, ".findex")));
        mg->forward_file.reset(MMapFile::Open(concat(prefix, ".forward")));
        mg->backward_index_file.reset(MMapFile::Open(concat(prefix, ".bindex")));
        mg->backward_file.reset(MMapFile::Open(concat(prefix, ".backward")));
        mg->vdata_file.reset(MMapFile::Open(concat(prefix, ".vdata")));
        mg->edata_file.reset(MMapFile::Open(concat(prefix, ".edata")));

        mg->g.meta = (GraphHeader*) mg->meta_file->Data();
        mg->g.findex = (vid_t*) mg->forward_index_file->Data();
        mg->g.forward = (EdgeListItem*) mg->forward_file->Data();
        mg->g.bindex = (vid_t*) mg->backward_index_file->Data();
        mg->g.backward = (EdgeListItem*) mg->backward_file->Data();
        mg->g.vertexData = (char*) mg->vdata_file->Data();
        mg->g.edgeData = (char*) mg->edata_file->Data();

        return mg;
    }

    static MappedGraphImpl* Create(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_size, uint32_t edge_data_size) {
        MappedGraphImpl* mg = new MappedGraphImpl();

        mg->meta_file.reset(MMapFile::Create(concat(prefix, ".meta"), sizeof(GraphHeader)));
        mg->forward_index_file.reset(MMapFile::Create(concat(prefix, ".findex"), sizeof(vid_t) * (n + 1)));
        mg->backward_index_file.reset(MMapFile::Create(concat(prefix, ".bindex"), sizeof(vid_t) * (n + 1)));
        mg->forward_file.reset(MMapFile::Create(concat(prefix, ".forward"), sizeof(EdgeListItem) * m));
        mg->backward_file.reset(MMapFile::Create(concat(prefix, ".backward"), sizeof(EdgeListItem) * m));
        mg->vdata_file.reset(MMapFile::Create(concat(prefix, ".vdata"), vertex_data_size * n));
        mg->edata_file.reset(MMapFile::Create(concat(prefix, ".edata"), edge_data_size * m));

        mg->g.meta = (GraphHeader*) mg->meta_file->Data();
        mg->g.findex = (vid_t*) mg->forward_index_file->Data();
        mg->g.bindex = (vid_t*) mg->backward_index_file->Data();
        mg->g.forward = (EdgeListItem*) mg->forward_file->Data();
        mg->g.backward = (EdgeListItem*) mg->backward_file->Data();
        mg->g.vertexData = (char*) mg->vdata_file->Data();
        mg->g.edgeData = (char*) mg->edata_file->Data();

        mg->g.meta->vertices = n;
        mg->g.meta->edges = m;
        mg->g.meta->vertex_data_size = vertex_data_size;
        mg->g.meta->edge_data_size = edge_data_size;

        return mg;
    }

    vid_t VertexCount() {
        return g.meta->vertices;
    }

    eid_t EdgeCount() {
        return g.meta->edges;
    }

    unique_ptr<VertexIterator> Vertices() {
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, 0, g.meta->vertices));
    }

    unique_ptr<EdgeIterator> ForwardEdges() {
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.meta->edges, g.forward));
    }

    unique_ptr<EdgeIterator> BackwardEdges() {
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.meta->edges, g.backward));
    }

    void Close() {
        if (meta_file) meta_file->Close();
        if (forward_index_file) forward_index_file->Close();
        if (backward_index_file) backward_index_file->Close();
        if (forward_file) forward_file->Close();
        if (backward_file) backward_file->Close();
        if (vdata_file) vdata_file->Close();
        if (edata_file) edata_file->Close();
        memset(&g, 0, sizeof(g));
    }
};

MappedGraph* MappedGraph::Open(const char * prefix) {
    return MappedGraphImpl::Open(prefix);
}

struct MappedGraphWriterImpl : public MappedGraphWriter {
    MappedGraphImpl *mg;
    GraphData *g;
    uint32_t edata_size, vdata_size;
    vid_t vi;
    eid_t ei;

    static MappedGraphWriter* Open(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_size, uint32_t edge_data_size) {
        auto mg = MappedGraphImpl::Create(prefix, n, m, vertex_data_size, edge_data_size);
        if (mg) {
            MappedGraphWriterImpl *mgw = new MappedGraphWriterImpl();
            mgw->mg = mg;
            mgw->g = &mg->g;
            mgw->edata_size = edge_data_size;
            mgw->vdata_size = vertex_data_size;
            return mgw;
        }
        return nullptr;
    }

    void AppendVertex(void *data) {
        memcpy(g->vertexData + vi * vdata_size, data, vdata_size);
        vi ++;
    }

    void AppendEdge(vid_t source, vid_t target, void* data) {
        memcpy(g->edgeData + ei * edata_size, data, edata_size);
        g->forward[ei] = g->backward[ei] = EdgeListItem{source, target, ei};
        ei ++;
    }

    void Close() {
        vid_t n = g->meta->vertices;
        eid_t m = g->meta->edges;

        // sort forward and backword edge lists
        qsort(g->forward, g->meta->edges, sizeof(EdgeListItem), source_comparer);
        qsort(g->backward, g->meta->edges, sizeof(EdgeListItem), target_comparer);

        // build index
        vid_t *f = g->findex;
        eid_t ei = 0;
        f[0] = 0;
        for (vid_t v = 0; v < n; v++) {
            while (ei < m && g->forward[ei].source == v) {
                ei ++;
            }
            f[v + 1] = ei;
        }

        vid_t *b = g->bindex;
        ei = 0;
        b[0] = 0;
        for (vid_t v = 0; v < n; v++) {
            while (ei < m && g->backward[ei].target == v) {
                ei ++;
            }
            b[v + 1] = ei;
        }

        mg->Close();
        delete mg;
    }
};

MappedGraphWriter* MappedGraphWriter::Open(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_size, uint32_t edge_data_size) {
    return MappedGraphWriterImpl::Open(prefix, n, m, vertex_data_size, edge_data_size);
}

} // io
} // sae
