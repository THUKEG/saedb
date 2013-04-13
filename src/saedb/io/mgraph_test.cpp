#include <iostream>
#include "mgraph.hpp"

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

    builder.Save("test_graph");
}

void test_append()
{
	MappedGraphWriter* writer = sae::io::MappedGraphWriter::Open("test_graph", 5, 4, sizeof(VData), sizeof(EData));
	VData vdata{0.8};
	//writer->AppendVertex(&vdata);
	writer->Close();
}

void test_load() {
    MappedGraph* g = MappedGraph::Open("test_graph");
    cout << "loaded, n: " << g->VertexCount() << ", m:" << g->EdgeCount() << endl;

    for (auto vs = g->Vertices(); vs->Alive(); vs->Next()) {
        VData* vd = (VData*) vs->Data();
        cout << vs->Id() << ": " << vd->pagerank << endl;

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

int main(int argc, const char * argv[]) {
    if (argc == 1 || (argc > 1 && argv[1][0] == 'c')) test_create();
	test_append();
    if (argc == 1 || (argc > 1 && argv[1][0] == 'l')) test_load();
}
