#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "mmap_file.hpp"
#include "mgraph.hpp"
#include "type_info.hpp"

#define MAX_EDGE_TYPE 5

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

struct DataTypeInfoItem {
    uint32_t data_size;
    uint32_t count;
    DataTypeInfoItem(uint32_t data_size, uint32_t count) :
        data_size(data_size), count(count) {}
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
    DataTypeInfoItem * vertex_data_type_info, * edge_data_type_info;
    char * vertex_data_type_accessor, * edge_data_type_accessor;
    vid_t ** vertex_type_list;
    EdgeListItem ** edge_type_list;
    char ** vertex_data;
    char ** edge_data;
};


// Evil forward declartion.
EdgeIteratorPtr CreateEdgeIterator(const GraphData& g, eid_t base, eid_t count, EdgeListItem* list, const std::vector<DataTypeAccessor*>&, const std::vector<DataTypeAccessor*>&);


struct VertexIteratorImpl : public VertexIterator {
    const GraphData& g;
    vid_t global_id;
    const std::vector<DataTypeAccessor*>& vertex_data_types;
    const std::vector<DataTypeAccessor*>& edge_data_types;

    VertexIteratorImpl(const GraphData& g, vid_t global_id, const std::vector<DataTypeAccessor*>& vertex_data_types, const std::vector<DataTypeAccessor*>& edge_data_types):
        g(g), global_id(global_id), vertex_data_types(vertex_data_types), edge_data_types(edge_data_types) {}
    
    vid_t GlobalId() {
        return global_id;
    }

    vid_t LocalId() {
        return g.vertex_list[global_id].local_id;
    }

    uint32_t DataTypeId() {
        return g.vertex_list[global_id].data_type;
    }

    void* Data() {
        return (void*)(g.vertex_data[DataTypeId()] + LocalId() * g.vertex_data_type_info[DataTypeId()].data_size);
    }

    void Next() {
        global_id ++;
    }

    void MoveTo(vid_t id) {
        global_id = id;
    }

    eid_t InEdgeCount() {
    // TODO what if global_id == vertices - 1
        return g.vertex_list[global_id + 1].bindex - g.vertex_list[global_id].bindex;
    }

    eid_t OutEdgeCount() {
    // TODO what if global_id == vertices - 1
        return g.vertex_list[global_id + 1].findex - g.vertex_list[global_id].findex;
    }

    EdgeIteratorPtr InEdges() {
        return CreateEdgeIterator(g, g.vertex_list[global_id].bindex, g.vertex_list[global_id + 1].bindex - g.vertex_list[global_id].bindex, g.backward, vertex_data_types, edge_data_types);
    }

    EdgeIteratorPtr OutEdges() {
        return CreateEdgeIterator(g, g.vertex_list[global_id].findex, g.vertex_list[global_id + 1].findex - g.vertex_list[global_id].findex, g.forward, vertex_data_types, edge_data_types);
    }

    EdgeIteratorPtr OutEdgesOfType(const char* type_name) {
        uint32_t edge_data_type_count = g.meta->edge_data_type_count;
        uint32_t edge_type_rank = -1;
        for (int i=0; i<edge_data_type_count; i++) {
            if (strcmp(edge_data_types[i]->getTypeName(), type_name) == 0) {
                edge_type_rank = i;
                break;
            }
        }
        if (edge_type_rank == -1) return nullptr;
        return CreateEdgeIterator(g, g.vertex_list[global_id].type_index[edge_type_rank], g.vertex_list[global_id + 1].type_index[edge_type_rank] - g.vertex_list[global_id].type_index[edge_type_rank], g.edge_type_list[edge_type_rank], vertex_data_types, edge_data_types);
    }

    bool Alive() {
        return global_id < g.meta->vertices;
    }

    vid_t Count() {
        return g.meta->vertices;
    }

    VertexIteratorPtr Clone() {
        auto p = new VertexIteratorImpl(g, global_id, vertex_data_types, edge_data_types);
        return VertexIteratorPtr(p);
    }
};


struct EdgeIteratorImpl : public EdgeIterator {
    const GraphData& g;
    EdgeListItem *list;
    eid_t base, index, count;
    const std::vector<DataTypeAccessor*>& vertex_data_types;
    const std::vector<DataTypeAccessor*>& edge_data_types;

    EdgeIteratorImpl(const GraphData& g, eid_t base, eid_t count, EdgeListItem* list, const std::vector<DataTypeAccessor*>& vertex_data_types, const std::vector<DataTypeAccessor*>& edge_data_types) :
        g(g), list(list), base(base), index(0), count(count), vertex_data_types(vertex_data_types), edge_data_types(edge_data_types) {}

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
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, source_id, vertex_data_types, edge_data_types));
    }

    VertexIteratorPtr Target() {
        vid_t target_id = TargetId();
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, target_id, vertex_data_types, edge_data_types));
    }

    void* Data() {
        return (void*)(g.edge_data[DataTypeId()] + LocalId() * g.edge_data_type_info[DataTypeId()].data_size);
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
        auto p = new EdgeIteratorImpl(g, base, count, list, vertex_data_types, edge_data_types);
        p->index = index;
        return EdgeIteratorPtr(p);
    }
};


EdgeIteratorPtr CreateEdgeIterator(const GraphData& g, eid_t base, eid_t count, EdgeListItem* list, const std::vector<DataTypeAccessor*>& vertex_data_types, const std::vector<DataTypeAccessor*>& edge_data_types) {
    return EdgeIteratorPtr(new EdgeIteratorImpl(g, base, count, list, vertex_data_types, edge_data_types));
}


struct MappedGraphImpl : public MappedGraph {
    unique_ptr<MMapFile> meta_file, forward_file, backward_file, vlist_file, elist_file, vtypeinfo_file, etypeinfo_file, vaccessor_file, eaccessor_file;
    unique_ptr<MMapFile> *vtypelist_file, *etypelist_file, *vdata_file, *edata_file;
    GraphData g;
    std::vector<DataTypeAccessor*> vertex_data_types;
    std::vector<DataTypeAccessor*> edge_data_types;


    static MappedGraphImpl* Open(const char * prefix) {
        MappedGraphImpl* mg = new MappedGraphImpl;

        mg->meta_file.reset(MMapFile::Open(concat(prefix, ".meta")));
        mg->forward_file.reset(MMapFile::Open(concat(prefix, ".forward")));
        mg->backward_file.reset(MMapFile::Open(concat(prefix, ".backward")));
        mg->vlist_file.reset(MMapFile::Open(concat(prefix, ".vlist")));
        mg->elist_file.reset(MMapFile::Open(concat(prefix, ".elist")));
        mg->vtypeinfo_file.reset(MMapFile::Open(concat(prefix, ".vtypeinfo")));
        mg->etypeinfo_file.reset(MMapFile::Open(concat(prefix, ".etypeinfo")));
        mg->vaccessor_file.reset(MMapFile::Open(concat(prefix, ".vaccessor")));
        mg->eaccessor_file.reset(MMapFile::Open(concat(prefix, ".eaccessor")));

        mg->g.meta = (GraphHeader*) mg->meta_file->Data();
        mg->g.forward = (EdgeListItem*) mg->forward_file->Data();
        mg->g.backward = (EdgeListItem*) mg->backward_file->Data();
        mg->g.vertex_list = (VertexListItem*) mg->vlist_file->Data();
        mg->g.edge_list = (EdgeListItem*) mg->elist_file->Data();
        mg->g.vertex_data_type_info = (DataTypeInfoItem*) mg->vtypeinfo_file->Data();
        mg->g.edge_data_type_info = (DataTypeInfoItem*) mg->etypeinfo_file->Data();
        mg->g.vertex_data_type_accessor = (char*) mg->vaccessor_file->Data();
        mg->g.edge_data_type_accessor = (char*) mg->eaccessor_file->Data();

        uint32_t vertex_data_type_count = mg->g.meta->vertex_data_type_count;
        mg->vtypelist_file = new unique_ptr<MMapFile>[vertex_data_type_count];
        mg->vdata_file = new unique_ptr<MMapFile>[vertex_data_type_count];
        mg->g.vertex_type_list = new vid_t*[vertex_data_type_count];
        mg->g.vertex_data = new char*[vertex_data_type_count];
        for (int i=0; i < vertex_data_type_count; i++) {
            mg->vtypelist_file[i].reset(MMapFile::Open(concat(prefix, concat(".vtypelist", i))));
            mg->vdata_file[i].reset(MMapFile::Open(concat(prefix, concat(".vdata", i))));
            mg->g.vertex_type_list[i] = (vid_t*) mg->vtypelist_file[i]->Data();
            mg->g.vertex_data[i] = (char*) mg->vdata_file[i]->Data();
        }

        uint32_t edge_data_type_count = mg->g.meta->edge_data_type_count;
        mg->etypelist_file = new unique_ptr<MMapFile>[edge_data_type_count];
        mg->edata_file = new unique_ptr<MMapFile>[edge_data_type_count];
        mg->g.edge_type_list = new EdgeListItem*[edge_data_type_count];
        mg->g.edge_data = new char*[edge_data_type_count];
        for (int i=0; i<edge_data_type_count; i++) {
            mg->etypelist_file[i].reset(MMapFile::Open(concat(prefix, concat(".etypelist", i))));
            mg->edata_file[i].reset(MMapFile::Open(concat(prefix, concat(".edata", i))));
            mg->g.edge_type_list[i] = (EdgeListItem*) mg->etypelist_file[i]->Data();
            mg->g.edge_data[i] = (char*) mg->edata_file[i]->Data();
        }
        
        DataTypeInfo* start = (DataTypeInfo*) mg->vaccessor_file->Data();
        for (int i=0; i< vertex_data_type_count; i++) {
            DataTypeInfo* dti = start;
            uint32_t fc = dti->field_num;
            FieldInfo* fi = (FieldInfo*)(start + 1);
            
            start = (DataTypeInfo*) (fi + fc);
            DataTypeAccessor* dt = DataTypeAccessorFactory(dti, fi);
            mg->vertex_data_types.push_back(dt);
        }

        start = (DataTypeInfo*) mg->eaccessor_file->Data();
        for (int i=0; i<edge_data_type_count; i++) {
            DataTypeInfo* dti = start;
            uint32_t fc = dti->field_num;
            FieldInfo* fi = (FieldInfo*) (start + 1);

            start = (DataTypeInfo*) (fi + fc);
            DataTypeAccessor* dt = DataTypeAccessorFactory(dti, fi);
            mg->edge_data_types.push_back(dt);
        }

        return mg;
    }

    static MappedGraphImpl* Create(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_type_count, uint32_t edge_data_type_count, uint32_t vertex_type_total_size, uint32_t edge_type_total_size, uint32_t * vertex_type_count, uint32_t * edge_type_count, uint32_t * vertex_data_size, uint32_t * edge_data_size) {
        MappedGraphImpl* mg = new MappedGraphImpl();

        mg->meta_file.reset(MMapFile::Create(concat(prefix, ".meta"), sizeof(GraphHeader)));
        mg->forward_file.reset(MMapFile::Create(concat(prefix, ".forward"), sizeof(EdgeListItem) * m));
        mg->backward_file.reset(MMapFile::Create(concat(prefix, ".backward"), sizeof(EdgeListItem) * m));
        mg->vlist_file.reset(MMapFile::Create(concat(prefix, ".vlist"), sizeof(VertexListItem) * n));
        mg->elist_file.reset(MMapFile::Create(concat(prefix, ".elist"), sizeof(EdgeListItem) * m));
        mg->vtypeinfo_file.reset(MMapFile::Create(concat(prefix, ".vtypeinfo"), sizeof(DataTypeInfoItem) * vertex_data_type_count));
        mg->etypeinfo_file.reset(MMapFile::Create(concat(prefix, ".etypeinfo"), sizeof(DataTypeInfoItem) * edge_data_type_count));
        mg->vaccessor_file.reset(MMapFile::Create(concat(prefix, ".vaccessor"), vertex_type_total_size));
        mg->eaccessor_file.reset(MMapFile::Create(concat(prefix, ".eaccessor"), edge_type_total_size));

        mg->vtypelist_file = new unique_ptr<MMapFile>[vertex_data_type_count];
        mg->vdata_file = new unique_ptr<MMapFile>[vertex_data_type_count];
        for (int i=0; i<vertex_data_type_count; i++) {
            mg->vtypelist_file[i].reset(MMapFile::Create(concat(prefix, concat(".vtypelist", i)), sizeof(vid_t) * vertex_type_count[i]));
            mg->vdata_file[i].reset(MMapFile::Create(concat(prefix, concat(".vdata", i)), vertex_data_size[i] * vertex_type_count[i]));
        }
        mg->etypelist_file = new unique_ptr<MMapFile>[edge_data_type_count];
        mg->edata_file = new unique_ptr<MMapFile>[edge_data_type_count];
        for (int i=0; i<edge_data_type_count; i++) {
            mg->etypelist_file[i].reset(MMapFile::Create(concat(prefix, concat(".etypelist", i)), sizeof(EdgeListItem) * edge_type_count[i]));
            mg->edata_file[i].reset(MMapFile::Create(concat(prefix, concat(".edata", i)), edge_data_size[i] * edge_type_count[i]));
        }

        mg->g.meta = (GraphHeader*) mg->meta_file->Data();
        mg->g.forward = (EdgeListItem*) mg->forward_file->Data();
        mg->g.backward = (EdgeListItem*) mg->backward_file->Data();
        mg->g.vertex_list = (VertexListItem*) mg->vlist_file->Data();
        mg->g.edge_list = (EdgeListItem*) mg->elist_file->Data();
        mg->g.vertex_data_type_info = (DataTypeInfoItem*) mg->vtypeinfo_file->Data();
        mg->g.edge_data_type_info = (DataTypeInfoItem*) mg->etypeinfo_file->Data();
        mg->g.vertex_data_type_accessor = (char*) mg->vaccessor_file->Data();
        mg->g.edge_data_type_accessor = (char*) mg->eaccessor_file->Data();

        // TODO plus one?
        mg->g.vertex_type_list = new vid_t*[vertex_data_type_count + 1];
        for (int i=0; i<vertex_data_type_count; i++) {
            mg->g.vertex_type_list[i] = (vid_t*) mg->vtypelist_file[i]->Data();
        }
        mg->g.edge_type_list = new EdgeListItem*[edge_data_type_count + 1];
        for (int i=0; i<edge_data_type_count; i++) {
            mg->g.edge_type_list[i] = (EdgeListItem*) mg->etypelist_file[i]->Data();
        }
        mg->g.vertex_data = new char*[vertex_data_type_count + 1];
        for (int i=0; i<vertex_data_type_count; i++) {
            mg->g.vertex_data[i] = (char*) mg->vdata_file[i]->Data();
        }
        mg->g.edge_data = new char*[edge_data_type_count + 1];
        for (int i=0; i<edge_data_type_count; i++) {
            mg->g.edge_data[i] = (char*) mg->edata_file[i]->Data();
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
            if (strcmp(type, vertex_data_types[i]->getTypeName()) == 0) {
                return g.vertex_data_type_info[i].count;
            }
        }
        return 0;
    }

    eid_t EdgeOfTypeCount(const char* type) {
        uint32_t edge_data_type_count = EdgeTypeCount();
        for (int i=0; i<edge_data_type_count; i++) {
            if (strcmp(type, edge_data_types[i]->getTypeName()) == 0) {
                return g.edge_data_type_info[i].count;
            }
        }
        return 0;
    }

    VertexIteratorPtr Vertices() {
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, 0, vertex_data_types, edge_data_types));
    }

    VertexIteratorPtr VerticesOfType(const char* type) {
        uint32_t vertex_data_type_count = VertexTypeCount();
        uint32_t type_rank = -1;
        for (int i=0; i<vertex_data_type_count; i++) {
            if (strcmp(type, vertex_data_types[i]->getTypeName()) == 0) {
                type_rank = i;
                break;
            }
        }
        if (type_rank == -1) return nullptr;
        return unique_ptr<VertexIterator>(new VertexIteratorImpl(g, g.vertex_type_list[type_rank][0], vertex_data_types, edge_data_types));
    }

    EdgeIteratorPtr Edges() {
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.meta->edges, g.edge_list, vertex_data_types, edge_data_types));
    }

    EdgeIteratorPtr ForwardEdges() {
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.meta->edges, g.forward, vertex_data_types, edge_data_types));
    }

    EdgeIteratorPtr BackwardEdges() {
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.meta->edges, g.backward, vertex_data_types, edge_data_types));
    }

    EdgeIteratorPtr EdgesOfType(const char* type_name) {
        uint32_t edge_data_type_count = EdgeTypeCount();
        uint32_t type_rank = -1;
        for (int i=0; i<edge_data_type_count; i++) {
            if (strcmp(type_name, edge_data_types[i]->getTypeName()) == 0) {
                type_rank = i;
                break;
            }
        }
        if (type_rank == -1) return nullptr;
        return unique_ptr<EdgeIterator>(new EdgeIteratorImpl(g, 0, g.edge_data_type_info[type_rank].count, g.edge_type_list[type_rank], vertex_data_types, edge_data_types));
    }

    DataTypeAccessor* VertexDataType(const char* name) {
        for (auto p: vertex_data_types) {
            if (strcmp(p->getTypeName(), name) == 0) {
                return p;
            }
        }
        return nullptr;
    }

    DataTypeAccessor* EdgeDataType(const char* name) {
        for (auto p: edge_data_types) {
            if (strcmp(p->getTypeName(), name) == 0) {
                return p;
            }
        }
        return nullptr;
    }

    std::vector<DataTypeAccessor*> VertexDataTypes() {
        return vertex_data_types;
    }

    std::vector<DataTypeAccessor*> EdgeDataTypes() {
        return edge_data_types;
    }

    void Sync() {
        meta_file->Sync();
        forward_file->Sync();
        backward_file->Sync();
        vlist_file->Sync();
        elist_file->Sync();
        vtypeinfo_file->Sync();
        etypeinfo_file->Sync();
        vaccessor_file->Sync();
        eaccessor_file->Sync();

        for (int i=0; i<g.meta->vertex_data_type_count; i++) {
            vtypelist_file[i]->Sync();
            vdata_file[i]->Sync();
        }
        for (int i=0; i<g.meta->edge_data_type_count; i++) {
            etypelist_file[i]->Sync();
            edata_file[i]->Sync();
        }
    }

    void Close() {
        int vertex_data_type_count = g.meta->vertex_data_type_count;
        int edge_data_type_count = g.meta->edge_data_type_count;
        if (meta_file) meta_file->Close();
        if (forward_file) forward_file->Close();
        if (backward_file) backward_file->Close();
        if (vlist_file) vlist_file->Close();
        if (elist_file) elist_file->Close();
        if (vtypeinfo_file) vtypeinfo_file->Close();
        if (etypeinfo_file) etypeinfo_file->Close();
        if (vaccessor_file) vaccessor_file->Close();
        if (eaccessor_file) eaccessor_file->Close();


        for (int i=0; i<vertex_data_type_count; i++) {
            if (vtypelist_file[i]) vtypelist_file[i]->Close();
            if (vdata_file[i]) vdata_file[i]->Close();
        }
        for (int i=0; i<edge_data_type_count; i++) {
            if (etypelist_file[i]) etypelist_file[i]->Close();
            if (edata_file[i]) edata_file[i]->Close();
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
    uint32_t edge_type_file_offset, vertex_type_file_offset;
    uint32_t eti, vti;

    static MappedGraphWriter* Open(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_type_count, uint32_t edge_data_type_count, uint32_t vertex_type_total_size, uint32_t edge_type_total_size, uint32_t * vertex_type_count, uint32_t * edge_type_count, uint32_t * vertex_data_size, uint32_t * edge_data_size) {
        auto mg = MappedGraphImpl::Create(prefix, n, m, vertex_data_type_count, edge_data_type_count, vertex_type_total_size, edge_type_total_size, vertex_type_count, edge_type_count, vertex_data_size, edge_data_size);
        if (mg) {
            MappedGraphWriterImpl *mgw = new MappedGraphWriterImpl();
            mgw->mg = mg;
            mgw->g = &mg->g;
            mgw->edge_type_file_offset = 0;
            mgw->vertex_type_file_offset = 0;
            mgw->eti = 0;
            mgw->vti = 0;
            return mgw;
        }
        return nullptr;
    }

    void AppendVertex(vid_t global_id, vid_t local_id, uint32_t type_name, void *data) {
        g->vertex_list[global_id] = VertexListItem(global_id, local_id, type_name);
        g->vertex_type_list[type_name][local_id] = global_id;
        memcpy(g->vertex_data[type_name] + local_id * g->vertex_data_type_info[type_name].data_size, data, g->vertex_data_type_info[type_name].data_size);
    }

    void AppendEdge(vid_t source, vid_t target, eid_t global_id, eid_t local_id, void* data, uint32_t type_name) {
        g->forward[global_id] = 
            g->backward[global_id] =
                g->edge_list[global_id] = 
                    g->edge_type_list[type_name][local_id] = EdgeListItem(global_id, local_id, type_name, source, target);
        memcpy(g->edge_data[type_name] + local_id * g->edge_data_type_info[type_name].data_size, data, g->edge_data_type_info[type_name].data_size);
    }

    void AppendVertexDataType(uint32_t data_size, DataTypeAccessor* accessor, uint32_t count) {
        g->vertex_data_type_info[vti ++] = DataTypeInfoItem(data_size, count);

        memcpy(g->vertex_data_type_accessor + vertex_type_file_offset, accessor->getTypeName(), sizeof(DataTypeInfo));
        vertex_type_file_offset += sizeof(DataTypeInfo);
        for (auto fi: accessor->getAllFields()) {
            memcpy(g->vertex_data_type_accessor + vertex_type_file_offset, fi, sizeof(FieldInfo));
            vertex_type_file_offset += sizeof(FieldInfo);
        }
    }

    void AppendEdgeDataType(uint32_t data_size, DataTypeAccessor* accessor, uint32_t count) {
        g->edge_data_type_info[eti ++] = DataTypeInfoItem(data_size, count);

        memcpy(g->edge_data_type_accessor + edge_type_file_offset, accessor->getTypeName(), sizeof(DataTypeInfo));
        edge_type_file_offset += sizeof(DataTypeInfo);
        for (auto fi: accessor->getAllFields()) {
            memcpy(g->edge_data_type_accessor + edge_type_file_offset, fi, sizeof(FieldInfo));
            edge_type_file_offset += sizeof(FieldInfo);
        }
    }

    void Close() {
        vid_t n = g->meta->vertices;
        eid_t m = g->meta->edges;

        // sort forward and backword edge lists
        std::qsort(g->forward, g->meta->edges, sizeof(EdgeListItem), source_comparer);
        std::qsort(g->backward, g->meta->edges, sizeof(EdgeListItem), target_comparer);

        // build index
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

MappedGraphWriter* MappedGraphWriter::Open(const char * prefix, vid_t n, eid_t m, uint32_t vertex_data_type_count, uint32_t edge_data_type_count, uint32_t vertex_type_total_size, uint32_t edge_type_total_size, uint32_t * vertex_type_count, uint32_t * edge_type_count, uint32_t * vertex_data_size, uint32_t * edge_data_size) {
    return MappedGraphWriterImpl::Open(prefix, n, m, vertex_data_type_count, edge_data_type_count, vertex_type_total_size, edge_type_total_size, vertex_type_count, edge_type_count, vertex_data_size, edge_data_size);
}

} // io
} // sae
