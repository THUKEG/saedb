#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include "../serialization/serialization_includes.hpp"

#include "mmap_file.hpp"
#include "mgraph.hpp"
#include "type_info.hpp"

#define MAX_EDGE_TYPE 5

struct DataTypeInfoItem {
    uint32_t count;
    std::string type_name;
    DataTypeInfoItem(uint32_t count = 0, std::string type_name = std::string("")) :
        count(count), type_name(type_name) {}
};

namespace sae {
    namespace serialization {
        namespace custom_serialization_impl {
            template <>
            struct serialize_impl<sae::serialization::OSerializeStream, DataTypeInfoItem> {
                static void run(sae::serialization::OSerializeStream& ostr, DataTypeInfoItem& d) {
                    ostr << d.count << d.type_name;
                }
            };

            template <>
            struct deserialize_impl<sae::serialization::ISerializeStream, DataTypeInfoItem> {
                static void run(sae::serialization::ISerializeStream& istr, DataTypeInfoItem& d) {
                    istr >> d.count >> d.type_name;
                }
            };
        }
    }
}

namespace sae {
namespace io {

using std::uint32_t;
using std::uint64_t;
using std::unique_ptr;


struct GraphHeader {
    vid_t vertices;
    eid_t edges;
    uint32_t vertex_data_type_count;
    uint32_t edge_data_type_count;
};

struct VertexListItem {
    vid_t global_id;
    vid_t local_id;
    uint32_t data_type;
    eid_t findex;
    eid_t bindex;
    eid_t type_index[MAX_EDGE_TYPE];// TODO enable flexible storage.
    VertexListItem(vid_t global_id, vid_t local_id, uint32_t data_type, eid_t findex = 0, eid_t bindex = 0):
        global_id(global_id), local_id(local_id), data_type(data_type), findex(findex), bindex(bindex) {}
};


struct EdgeListItem {
    eid_t global_id;
    eid_t local_id;
    uint32_t data_type;
    vid_t source_id;
    vid_t target_id;
    EdgeListItem(eid_t global_id, eid_t local_id, uint32_t data_type, vid_t source_id, vid_t target_id):
        global_id(global_id), local_id(local_id), data_type(data_type), source_id(source_id), target_id(target_id) {}
};

namespace {

    const char * concat(const char * a, const char * b) {
        static char buf[1024];
        std::snprintf(buf, sizeof(buf), "%s%s", a, b);
        return buf;
    }

    const char * concat(const char * a, const int b) {
        static char buf[1024];
        std::snprintf(buf, sizeof(buf), "%s%d", a, b);
        return buf;
    }

    int source_comparer(const void * a, const void * b) {
        EdgeListItem* ea = (EdgeListItem*) a;
        EdgeListItem* eb = (EdgeListItem*) b;
        return ea->source_id - eb->source_id;
    }

    int target_comparer(const void * a, const void * b) {
        EdgeListItem* ea = (EdgeListItem*) a;
        EdgeListItem* eb = (EdgeListItem*) b;
        return ea->target_id - eb->target_id;
    }
}


struct GraphData {
    GraphHeader * meta;
    EdgeListItem * forward, * backward;
    VertexListItem * vertex_list;
    EdgeListItem * edge_list;
    std::vector<DataTypeInfoItem> vertex_data_type_info, edge_data_type_info;
    vid_t ** vertex_type_list;
    EdgeListItem ** edge_type_list;
    std::vector<std::string> * vertex_data;
    std::vector<std::string> * edge_data;
};


// Evil forward declartion.
EdgeIteratorPtr CreateEdgeIterator(const GraphData& g, eid_t base, eid_t count, EdgeListItem* list);


struct VertexIteratorImpl : public VertexIterator {
    const GraphData& g;
    vid_t global_id;

    VertexIteratorImpl(const GraphData& g, vid_t global_id):
        g(g), global_id(global_id) {}
    
    vid_t GlobalId() {
        return global_id;
    }

    vid_t LocalId() {
        return g.vertex_list[global_id].local_id;
    }

    uint32_t DataTypeId() {
        return g.vertex_list[global_id].data_type;
    }

    std::string Data() {
        return g.vertex_data[DataTypeId()][LocalId()];
    }

    void Next() {
        global_id ++;
    }

    void NextOfType() {
        if (LocalId() == g.vertex_data_type_info[DataTypeId()].count - 1) {
            MoveTo(g.meta->vertices);
            return;
        }
        MoveTo(g.vertex_type_list[DataTypeId()][LocalId() + 1]);
    }

    void MoveTo(vid_t id) {
        global_id = id;
    }

    eid_t InEdgeCount() {
        return g.vertex_list[global_id + 1].bindex - g.vertex_list[global_id].bindex;
    }

    eid_t OutEdgeCount() {
        return g.vertex_list[global_id + 1].findex - g.vertex_list[global_id].findex;
    }

    EdgeIteratorPtr InEdges() {
        return CreateEdgeIterator(g, g.vertex_list[global_id].bindex, g.vertex_list[global_id + 1].bindex - g.vertex_list[global_id].bindex, g.backward);
    }

    EdgeIteratorPtr OutEdges() {
        return CreateEdgeIterator(g, g.vertex_list[global_id].findex, g.vertex_list[global_id + 1].findex - g.vertex_list[global_id].findex, g.forward);
    }

    EdgeIteratorPtr OutEdgesOfType(const char* type_name) {
        uint32_t edge_data_type_count = g.meta->edge_data_type_count;
        uint32_t edge_type_rank = -1;
        for (int i=0; i<edge_data_type_count; i++) {
            if (g.edge_data_type_info[i].type_name == std::string(type_name)) {
                edge_type_rank = i;
                break;
            }
        }
        if (edge_type_rank == -1) return nullptr;
        return CreateEdgeIterator(g, g.vertex_list[global_id].type_index[edge_type_rank], g.vertex_list[global_id + 1].type_index[edge_type_rank] - g.vertex_list[global_id].type_index[edge_type_rank], g.edge_type_list[edge_type_rank]);
    }

    bool Alive() {
        return global_id < g.meta->vertices;
    }

    vid_t Count() {
        return g.meta->vertices;
    }

    VertexIteratorPtr Clone() {
        auto p = new VertexIteratorImpl(g, global_id);
        return VertexIteratorPtr(p);
    }
};


struct EdgeIteratorImpl : public EdgeIterator {
    const GraphData& g;
    EdgeListItem *list;
    eid_t base, index, count;

    EdgeIteratorImpl(const GraphData& g, eid_t base, eid_t count, EdgeListItem* list) :
        g(g), list(list), base(base), index(0), count(count) {}

    eid_t GlobalId() {
        return list[base + index].global_id;
    }

    eid_t LocalId() {
        return list[base + index].local_id;
    }

    vid_t SourceId() {
        return list[base + index].source_id;
    }

    vid_t TargetId() {
        return list[base + index].target_id;
    }
    
    uint32_t DataTypeId() {
        return list[base + index].data_type;
    }

    VertexIteratorPtr Source() {
        vid_t source_id = SourceId();
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, source_id));
    }

    VertexIteratorPtr Target() {
        vid_t target_id = TargetId();
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, target_id));
    }

    std::string Data() {
        return g.edge_data[DataTypeId()][LocalId()];
    }

    void Next() {
        index ++;
    }

    bool Alive() {
        return index < count;
    }

    eid_t Count() {
        return count;
    }

    EdgeIteratorPtr Clone() {
        auto p = new EdgeIteratorImpl(g, base, count, list);
        p->index = index;
        return EdgeIteratorPtr(p);
    }
};


EdgeIteratorPtr CreateEdgeIterator(const GraphData& g, eid_t base, eid_t count, EdgeListItem* list) {
    return EdgeIteratorPtr(new EdgeIteratorImpl(g, base, count, list));
}


struct MappedGraphImpl : public MappedGraph {
    unique_ptr<MMapFile> meta_file, forward_file, backward_file, vlist_file, elist_file;
    unique_ptr<MMapFile> *vtypelist_file, *etypelist_file, *vdata_file, *edata_file;
    GraphData g;
    char* my_prefix;


    static MappedGraphImpl* Open(const char * prefix) {
        MappedGraphImpl* mg = new MappedGraphImpl;

        int len = strlen(prefix);
        mg->my_prefix = new char[len + 1];
        strcpy(mg->my_prefix, prefix);

        mg->meta_file.reset(MMapFile::Open(concat(prefix, ".meta")));
        mg->forward_file.reset(MMapFile::Open(concat(prefix, ".forward")));
        mg->backward_file.reset(MMapFile::Open(concat(prefix, ".backward")));
        mg->vlist_file.reset(MMapFile::Open(concat(prefix, ".vlist")));
        mg->elist_file.reset(MMapFile::Open(concat(prefix, ".elist")));

        mg->g.meta = (GraphHeader*) mg->meta_file->Data();
        mg->g.forward = (EdgeListItem*) mg->forward_file->Data();
        mg->g.backward = (EdgeListItem*) mg->backward_file->Data();
        mg->g.vertex_list = (VertexListItem*) mg->vlist_file->Data();
        mg->g.edge_list = (EdgeListItem*) mg->elist_file->Data();

        std::ifstream finv(concat(prefix, ".vtypeinfo"));
        sae::serialization::ISerializeStream decoderv(&finv);
        decoderv >> mg->g.vertex_data_type_info;
        finv.close();

        std::ifstream fine(concat(prefix, ".etypeinfo"));
        sae::serialization::ISerializeStream decodere(&fine);
        decodere >> mg->g.edge_data_type_info;
        fine.close();

        uint32_t vertex_data_type_count = mg->g.meta->vertex_data_type_count;
        mg->vtypelist_file = new unique_ptr<MMapFile>[vertex_data_type_count];
        mg->g.vertex_type_list = new vid_t*[vertex_data_type_count];
        mg->g.vertex_data = new std::vector<std::string>[vertex_data_type_count];
        for (int i=0; i < vertex_data_type_count; i++) {
            mg->vtypelist_file[i].reset(MMapFile::Open(concat(prefix, concat(".vtypelist", i))));
            mg->g.vertex_type_list[i] = (vid_t*) mg->vtypelist_file[i]->Data();

            std::ifstream fin(concat(prefix, concat(".vdata", i)), std::fstream::binary);
            sae::serialization::ISerializeStream decoder(&fin);
            decoder >> mg->g.vertex_data[i];
            fin.close();
        }

        uint32_t edge_data_type_count = mg->g.meta->edge_data_type_count;
        mg->etypelist_file = new unique_ptr<MMapFile>[edge_data_type_count];
        mg->g.edge_type_list = new EdgeListItem*[edge_data_type_count];
        mg->g.edge_data = new std::vector<std::string>[edge_data_type_count];
        for (int i=0; i<edge_data_type_count; i++) {
            mg->etypelist_file[i].reset(MMapFile::Open(concat(prefix, concat(".etypelist", i))));
            mg->g.edge_type_list[i] = (EdgeListItem*) mg->etypelist_file[i]->Data();
            
            std::ifstream fin(concat(prefix, concat(".edata", i)), std::fstream::binary);
            sae::serialization::ISerializeStream decoder(&fin);
            decoder >> mg->g.edge_data[i];
            fin.close();
        }
        
        return mg;
    }

    static MappedGraphImpl* Create(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_type_count, uint32_t edge_data_type_count, uint32_t * vertex_type_count, uint32_t * edge_type_count) {
        MappedGraphImpl* mg = new MappedGraphImpl();

        int len = strlen(prefix);
        mg->my_prefix = new char[len + 1];
        strcpy(mg->my_prefix, prefix);

        mg->meta_file.reset(MMapFile::Create(concat(prefix, ".meta"), sizeof(GraphHeader)));
        mg->forward_file.reset(MMapFile::Create(concat(prefix, ".forward"), sizeof(EdgeListItem) * m));
        mg->backward_file.reset(MMapFile::Create(concat(prefix, ".backward"), sizeof(EdgeListItem) * m));
        mg->vlist_file.reset(MMapFile::Create(concat(prefix, ".vlist"), sizeof(VertexListItem) * (n + 1)));
        mg->elist_file.reset(MMapFile::Create(concat(prefix, ".elist"), sizeof(EdgeListItem) * m));

        mg->g.vertex_data_type_info.resize(vertex_data_type_count);
        mg->g.edge_data_type_info.resize(edge_data_type_count);

        mg->vtypelist_file = new unique_ptr<MMapFile>[vertex_data_type_count];
        for (int i=0; i<vertex_data_type_count; i++) {
            mg->vtypelist_file[i].reset(MMapFile::Create(concat(prefix, concat(".vtypelist", i)), sizeof(vid_t) * vertex_type_count[i]));
        }
        mg->etypelist_file = new unique_ptr<MMapFile>[edge_data_type_count];
        for (int i=0; i<edge_data_type_count; i++) {
            mg->etypelist_file[i].reset(MMapFile::Create(concat(prefix, concat(".etypelist", i)), sizeof(EdgeListItem) * edge_type_count[i]));
        }

        mg->g.meta = (GraphHeader*) mg->meta_file->Data();
        mg->g.forward = (EdgeListItem*) mg->forward_file->Data();
        mg->g.backward = (EdgeListItem*) mg->backward_file->Data();
        mg->g.vertex_list = (VertexListItem*) mg->vlist_file->Data();
        mg->g.edge_list = (EdgeListItem*) mg->elist_file->Data();

        mg->g.vertex_type_list = new vid_t*[vertex_data_type_count];
        for (int i=0; i<vertex_data_type_count; i++) {
            mg->g.vertex_type_list[i] = (vid_t*) mg->vtypelist_file[i]->Data();
        }
        mg->g.edge_type_list = new EdgeListItem*[edge_data_type_count];
        for (int i=0; i<edge_data_type_count; i++) {
            mg->g.edge_type_list[i] = (EdgeListItem*) mg->etypelist_file[i]->Data();
        }
        mg->g.vertex_data = new std::vector<std::string>[vertex_data_type_count];
        for (int i=0; i<vertex_data_type_count; i++) {
            mg->g.vertex_data[i].resize(vertex_type_count[i]);
        }
        mg->g.edge_data = new std::vector<std::string>[edge_data_type_count];
        for (int i=0; i<edge_data_type_count; i++) {
            mg->g.edge_data[i].resize(edge_type_count[i]);
        }

        mg->g.meta->vertices = n;
        mg->g.meta->edges = m;
        mg->g.meta->vertex_data_type_count = vertex_data_type_count;
        mg->g.meta->edge_data_type_count = edge_data_type_count;

        return mg;
    }

    vid_t VertexCount() {
        return g.meta->vertices;
    }

    eid_t EdgeCount() {
        return g.meta->edges;
    }
    
    uint32_t VertexTypeCount() {
        return g.meta->vertex_data_type_count;
    }

    uint32_t EdgeTypeCount() {
        return g.meta->edge_data_type_count;
    }

    vid_t VertexOfTypeCount(const char* type) {
        uint32_t vertex_data_type_count = VertexTypeCount();
        for (int i=0; i<vertex_data_type_count; i++) {
            if (g.vertex_data_type_info[i].type_name == std::string(type)) {
                return g.vertex_data_type_info[i].count;
            }
        }
        return 0;
    }

    eid_t EdgeOfTypeCount(const char* type) {
        uint32_t edge_data_type_count = EdgeTypeCount();
        for (int i=0; i<edge_data_type_count; i++) {
            if (g.edge_data_type_info[i].type_name == std::string(type)) {
                return g.edge_data_type_info[i].count;
            }
        }
        return 0;
    }

    VertexIteratorPtr Vertices() {
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, 0));
    }

    VertexIteratorPtr VerticesOfType(const char* type) {
        uint32_t vertex_data_type_count = VertexTypeCount();
        uint32_t type_rank = -1;
        for (int i=0; i<vertex_data_type_count; i++) {
            if (g.vertex_data_type_info[i].type_name == std::string(type)) {
                type_rank = i;
                break;
            }
        }
        if (type_rank == -1) return nullptr;
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, g.vertex_type_list[type_rank][0]));
    }

    EdgeIteratorPtr Edges() {
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.meta->edges, g.edge_list));
    }

    EdgeIteratorPtr ForwardEdges() {
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.meta->edges, g.forward));
    }

    EdgeIteratorPtr BackwardEdges() {
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.meta->edges, g.backward));
    }

    EdgeIteratorPtr EdgesOfType(const char* type_name) {
        uint32_t edge_data_type_count = EdgeTypeCount();
        uint32_t type_rank = -1;
        for (int i=0; i<edge_data_type_count; i++) {
            if (g.edge_data_type_info[i].type_name == std::string(type_name)) {
                type_rank = i;
                break;
            }
        }
        if (type_rank == -1) return nullptr;
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.edge_data_type_info[type_rank].count, g.edge_type_list[type_rank]));
    }

    void Sync() {
        meta_file->Sync();
        forward_file->Sync();
        backward_file->Sync();
        vlist_file->Sync();
        elist_file->Sync();
    }

    void Close() {
        int vertex_data_type_count = g.meta->vertex_data_type_count;
        int edge_data_type_count = g.meta->edge_data_type_count;
        if (meta_file) meta_file->Close();
        if (forward_file) forward_file->Close();
        if (backward_file) backward_file->Close();
        if (vlist_file) vlist_file->Close();
        if (elist_file) elist_file->Close();

        std::ofstream foutv(concat(my_prefix, ".vtypeinfo"));
        sae::serialization::OSerializeStream encoderv(&foutv);
        encoderv << g.vertex_data_type_info;
        foutv.close();

        std::ofstream foute(concat(my_prefix, ".etypeinfo"));
        sae::serialization::OSerializeStream encodere(&foute);
        encodere << g.edge_data_type_info;
        foute.close();

        for (int i=0; i<vertex_data_type_count; i++) {
            if (vtypelist_file[i]) vtypelist_file[i]->Close();
            std::ofstream fout(concat(my_prefix, concat(".vdata", i)), std::fstream::binary);
            sae::serialization::OSerializeStream encoder(&fout);
            encoder << g.vertex_data[i];
            fout.close();
        }
        for (int i=0; i<edge_data_type_count; i++) {
            if (etypelist_file[i]) etypelist_file[i]->Close();
            std::ofstream fout(concat(my_prefix, concat(".edata", i)), std::fstream::binary);
            sae::serialization::OSerializeStream encoder(&fout);
            encoder << g.edge_data[i];
            fout.close();
        }

        memset(&g, 0, sizeof(g));
    }
};

MappedGraph* MappedGraph::Open(const char * prefix) {
    return MappedGraphImpl::Open(prefix);
}

struct MappedGraphWriterImpl : public MappedGraphWriter {
    MappedGraphImpl *mg;
    GraphData *g;
    uint32_t eti, vti;

    static MappedGraphWriter* Open(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_type_count, uint32_t edge_data_type_count, uint32_t * vertex_type_count, uint32_t * edge_type_count) {
        auto mg = MappedGraphImpl::Create(prefix, n, m, vertex_data_type_count, edge_data_type_count, vertex_type_count, edge_type_count);
        if (mg) {
            MappedGraphWriterImpl *mgw = new MappedGraphWriterImpl();
            mgw->mg = mg;
            mgw->g = &mg->g;
            mgw->eti = 0;
            mgw->vti = 0;
            return mgw;
        }
        return nullptr;
    }

    void AppendVertex(vid_t global_id, vid_t local_id, uint32_t type_name, std::string data) {
        g->vertex_list[global_id] = VertexListItem(global_id, local_id, type_name);
        g->vertex_type_list[type_name][local_id] = global_id;
        g->vertex_data[type_name][local_id] = data;
    }

    void AppendEdge(vid_t source, vid_t target, eid_t global_id, eid_t local_id, std::string data, uint32_t type_name) {
        g->forward[global_id] = 
            g->backward[global_id] =
                g->edge_list[global_id] = 
                    g->edge_type_list[type_name][local_id] = EdgeListItem(global_id, local_id, type_name, source, target);
        g->edge_data[type_name][local_id] = data;
    }

    void AppendVertexDataType(std::string type_name, uint32_t count) {
        g->vertex_data_type_info[vti ++] = DataTypeInfoItem(count, type_name);
    }

    void AppendEdgeDataType(std::string type_name, uint32_t count) {
        g->edge_data_type_info[eti ++] = DataTypeInfoItem(count, type_name);
    }

    void Close() {
        vid_t n = g->meta->vertices;
        eid_t m = g->meta->edges;

        g->vertex_list[n] = VertexListItem(n, 0, 0);

        std::qsort(g->forward, g->meta->edges, sizeof(EdgeListItem), source_comparer);
        std::qsort(g->backward, g->meta->edges, sizeof(EdgeListItem), target_comparer);

        eid_t ei = 0;
        g->vertex_list[0].findex = 0;
        for (vid_t v = 0; v < n; v++) {
            while (ei < m && g->forward[ei].source_id == v) {
                ei ++;
            }
            g->vertex_list[v + 1].findex = ei;
        }

        ei = 0;
        g->vertex_list[0].bindex = 0;
        for (vid_t v = 0; v < n; v++) {
            while (ei < m && g->backward[ei].target_id == v) {
                ei ++;
            }
            g->vertex_list[v + 1].bindex = ei;
        }

        uint32_t edge_data_type_count = g->meta->edge_data_type_count;
        for (int i=0; i<edge_data_type_count; i++) {
            eid_t edge_count = g->edge_data_type_info[i].count;
            std::qsort(g->edge_type_list[i], edge_count, sizeof(EdgeListItem), source_comparer);
            g->vertex_list[0].type_index[i] = 0;
            for (vid_t v=0; v<n; v++) {
                while (ei < edge_count && g->edge_type_list[i][ei].source_id == v) {
                    ei ++;
                }
                g->vertex_list[v + 1].type_index[i] = ei;
            }
        }

        mg->Close();
        delete mg;
    }
};

MappedGraphWriter* MappedGraphWriter::Open(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_type_count, uint32_t edge_data_type_count, uint32_t * vertex_type_count, uint32_t * edge_type_count) {
    return MappedGraphWriterImpl::Open(prefix, n, m, vertex_data_type_count, edge_data_type_count, vertex_type_count, edge_type_count);
}

} // io
} // sae
