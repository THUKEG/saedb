#include <iostream>
#include "../graph.hpp"
#include "../ifilterquery.hpp"

using namespace std;
using namespace sae::io;

struct VData {
    double pagerank;
};

struct EData {
    int type;
};

void test_create() {
    sae::io::GraphBuilder<int, VData, EData> builder;

    builder.AddEdge(0, 10, EData{10});
    builder.AddEdge(10, 20, EData{20});
    builder.AddEdge(20, 30, EData{30});
    builder.AddEdge(30, 40, EData{40});

    builder.AddVertex(0, VData{0.5});
    builder.AddVertex(10, VData{0.6});
    builder.AddVertex(30, VData{0.7});

    DataTypeAccessor* vd = builder.CreateType("VData");
    std::cout << "Building type : " << vd->dt->type_name << std::endl;
    vd->appendField("pagerank", DOUBLE_T);
    builder.SaveDataType(vd);

    DataTypeAccessor* ed = builder.CreateType("EData");
    std::cout << "Building type : " << ed->dt->type_name << std::endl;
    ed->appendField("type", INT_T);
    builder.SaveDataType(ed);

    builder.Save("test_graph");
}

void test_load(const char* graph_name) {
    MappedGraph* g = MappedGraph::Open(graph_name);
    cout << "loaded, n: " << g->VertexCount() << ", m:" << g->EdgeCount() << endl;

    auto vtype = g->DataType("VData");
    for (auto vs = g->Vertices(); vs->Alive(); vs->Next()) {
        void* vd = vs->Data();
        cout << vs->Id() << ": " << vtype->getField<double>(vd, "pagerank") << endl;

        cout << "In Edges:" << endl;
        for (auto ei = vs->InEdges(); ei->Alive(); ei->Next()) {
            cout << "\t" << "[" << ei->SourceId() << "," << ei->TargetId() << "]" << ": " << ((EData*)ei->Data())->type << endl;
        }

        cout << "Out Edges:" << endl;
        for (auto ei = vs->OutEdges(); ei->Alive(); ei->Next()) {
            cout << "\t" << "[" << ei->SourceId() << "," << ei->TargetId() << "]" << ": " << ((EData*)ei->Data())->type << endl;
        }
    }

    cout << endl;
    cout << "Forward Edges:" << endl;

    auto etype = g->DataType("EData");
    for (auto es = g->ForwardEdges(); es->Alive(); es->Next()) {
        EData* ed = (EData*) es->Data();
        cout << "\t" << es->Id() << "[" << es->Source()->Id() << "," << es->Target()->Id() << "]" << ": " << ed->type << endl;
    }

    cout << "Backward Edges:" << endl;
    for (auto es = g->BackwardEdges(); es->Alive(); es->Next()) {
        EData* ed = (EData*) es->Data();
        cout << "\t" << es->Id() << "[" << es->Source()->Id() << "," << es->Target()->Id() << "]" << ": " << ed->type << endl;
    }

    g->Close();
    delete g;
}

void test_show_meta_information(const char* graph_name) {
    MappedGraph* g = MappedGraph::Open(graph_name);
    cout << "loaded, n: " << g->VertexCount() << ", m:" << g->EdgeCount() << endl;

    std::vector<DataTypeAccessor*> dataTypes = g->DataTypes();
    for (auto p : dataTypes) {
        cout << "struct " << p->dt->type_name << " :" << endl;
        auto fields = p->getAllFields();
        for (auto f : fields) {
            cout << "\tname:" << f->field_name << " offset:" << f->offset << " size:" << f->size << endl;
        }
        cout << endl;
    }
}

/**************************************************
 * Example of filter usage
 *
 * VData: old vertex data type
 * EData: old edge data type
 * vertex_data_type: new vertex data type
 * edge_data_type: new edge data type
 * vertex_predicate: predicate for subgraph selection, fromat: bool func_name(old_vertex_data_type*)
 * vertex_transform: old_vertex_data_type -> new_vertex_data_type, format: new_vertex_data_type func_name(old_vertex_data_type*)
 * edge_transform: old_edge_data_type -> new_edge_data_type, format: new_edge_data_type func_name(old_edge_data_type*)
 **************************************************/
typedef double vertex_data_type;
typedef int edge_data_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

struct FilterQuery : public IFilterQuery<vertex_data_type, edge_data_type, VData, EData>{
    bool vertex_predicate(VData* v){
        return v->pagerank > 0.3;
    }

    vertex_data_type vertex_transform(const VData* v){
        return v->pagerank;
    }

    edge_data_type edge_transform(const EData* e){
        return e->type;
    }
};

void test_filter(){
    std::cout << "Test filter" << std::endl;
    std::string graph_path = "test_graph";

    // This is the source graph
    saedb::sae_graph<VData, EData> from_graph;
    from_graph.load_format(graph_path);

    // This is the target graph builder
    sae::io::GraphBuilder<saedb::vertex_id_type, vertex_data_type, edge_data_type> builder;

    FilterQuery query;
    //from_graph.filter(builder, vertex_predicate, vertex_transform, edge_transform);
    from_graph.filter(builder, query);
    // now builder has the needed vertex and edge data to build graph.
    builder.Save("test_graph_transformed");
}

int main(int argc, const char * argv[]) {
    if (argc == 1 || (argc > 1 && argv[1][0] == 'c')) test_create();
    if (argc == 3 && argv[1][0] == 'l') test_load(argv[2]);
    if (argc == 1 || (argc > 1 && argv[1][0] == 'f')) test_filter();
    if (argc == 3 && argv[1][0] == 's') test_show_meta_information(argv[2]);
}
