#include <iostream>
#include <sstream>
#include <cstring>
#include "../ifilterquery.hpp"
#include "../graph.hpp"
#include "../serialization/serialization_includes.hpp"

using namespace std;
using namespace sae::io;
using namespace sae::serialization;

struct VData {
    double pagerank;
};

struct EData {
    int type;
};

struct VData2 {
    int number;
    std::string name;
};

namespace sae {
    namespace serialization {
        namespace custom_serialization_impl {
            template <>
            struct serialize_impl<OSerializeStream, VData2> {
                static void run(OSerializeStream& ostr, VData2& d) {
                    ostr << d.number << d.name;
                }
            };
            template <>
            struct deserialize_impl<ISerializeStream, VData2> {
                static void run(ISerializeStream& istr, VData2& d) {
                    istr >> d.number >> d.name;
                }
            };
        }
    }
}

void test_create() {
    sae::io::GraphBuilder<int> builder;

    builder.AddVertexDataType("VData");
    builder.AddVertexDataType("VData2");

    builder.AddEdgeDataType("EData");

    std::cout << "Adding Vertices..." << std::endl;
    builder.AddVertex(0, double(0.5), "VData");
    builder.AddVertex(20, double(0.4), "VData");
    builder.AddVertex(10, double(0.6), "VData");
    builder.AddVertex(30, VData2{7, "kimi"}, "VData2");
    builder.AddVertex(40, VData2{8, "young"}, "VData2");

    std::cout << "Adding Edges..." << std::endl;
    builder.AddEdge(0, 10, EData{10}, "EData");
    builder.AddEdge(10, 20, EData{20}, "EData");
    builder.AddEdge(20, 30, EData{30}, "EData");
    builder.AddEdge(30, 40, EData{40}, "EData");

    std::cout << "Saving the graph..." << std::endl;
    builder.Save("test_graph");
}

void test_load(const char* graph_name) {
    MappedGraph* g = MappedGraph::Open(graph_name);
    cout << "loaded, n: " << g->VertexCount() << ", m:" << g->EdgeCount() << endl;

    for (auto vs = g->VerticesOfType("VData"); vs->Alive(); vs->NextOfType()) {
        //std::cout << "here" << std::endl;
        double vd = sae::serialization::convert_from_string<double>(vs->Data());
        cout << vs->GlobalId() << ": " << vd << endl;

        cout << "In Edges:" << endl;
        for (auto ei = vs->InEdges(); ei->Alive(); ei->Next()) {
            cout << "\t" << "[" << ei->SourceId() << "," << ei->TargetId() << "]" << ": " << (sae::serialization::convert_from_string<EData>(ei->Data())).type << endl;
        }

        cout << "Out Edges:" << endl;
        for (auto ei = vs->OutEdges(); ei->Alive(); ei->Next()) {
            cout << "\t" << "[" << ei->SourceId() << "," << ei->TargetId() << "]" << ": " << (sae::serialization::convert_from_string<EData>(ei->Data())).type << endl;
        }
    }
    for (auto vs = g->VerticesOfType("VData2"); vs->Alive(); vs->NextOfType()) {
        VData2 vd = sae::serialization::convert_from_string<VData2>(vs->Data());
        cout << vs->GlobalId() << ": " << vd.number << ' ' << vd.name << endl;
    }

    cout << endl;
    cout << "Forward Edges:" << endl;

    for (auto es = g->ForwardEdges(); es->Alive(); es->Next()) {
        EData ed = sae::serialization::convert_from_string<EData>(es->Data());
        cout << "\t" << es->GlobalId() << "[" << es->Source()->GlobalId() << "," << es->Target()->GlobalId() << "]" << ": " << ed.type  << endl;
    }

    cout << "Backward Edges:" << endl;
    for (auto es = g->BackwardEdges(); es->Alive(); es->Next()) {
        EData ed = sae::serialization::convert_from_string<EData>(es->Data());
        cout << "\t" << es->GlobalId() << "[" << es->Source()->GlobalId() << "," << es->Target()->GlobalId() << "]" << ": " << ed.type << endl;
    }

    g->Close();
    delete g;
}
/*
void test_show_meta_information(const char* graph_name) {
    MappedGraph* g = MappedGraph::Open(graph_name);
    cout << "loaded, n: " << g->VertexCount() << ", m:" << g->EdgeCount() << endl;

    std::vector<DataTypeAccessor*> dataTypes = g->VertexDataTypes();
    for (auto p : dataTypes) {
        cout << "struct " << p->getTypeName() << " :" << endl;
        auto fields = p->getAllFields();
        for (auto f : fields) {
            cout << "\tname:" << f->field_name << " offset:" << f->offset << " size:" << f->size << endl;
        }
        cout << endl;
    }

    dataTypes = g->EdgeDataTypes();
    for (auto p : dataTypes) {
        cout << "struct " << p->getTypeName() << " :" << endl;
        auto fields = p->getAllFields();
        for (auto f : fields) {
            cout << "\tname:" << f->field_name << " offset:" << f->offset << " size:" << f->size << endl;
        }
        cout << endl;
    }
}*/

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
typedef saedb::sae_graph graph_type;

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
    /*
    std::cout << "Test filter" << std::endl;
    std::string graph_path = "test_graph";

    // This is the source graph
    saedb::sae_graph<VData, EData> from_graph;
    from_graph.load_format(graph_path);

    // This is the target graph builder
    sae::io::GraphBuilder<saedb::vertex_id_type> builder;

    FilterQuery query;
    //from_graph.filter(builder, vertex_predicate, vertex_transform, edge_transform);
    from_graph.filter(builder, query);
    // now builder has the needed vertex and edge data to build graph.
    builder.Save("test_graph_transformed");
    */
}

int main(int argc, const char * argv[]) {
    if (argc == 1 || (argc > 1 && argv[1][0] == 'c')) test_create();
    if (argc == 3 && argv[1][0] == 'l') test_load(argv[2]);
    //if (argc == 1 || (argc > 1 && argv[1][0] == 'f')) test_filter();
    //if (argc == 3 && argv[1][0] == 's') test_show_meta_information(argv[2]);
}
