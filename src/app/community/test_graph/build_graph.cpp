#include <iostream>
#include "io/mgraph.hpp"

using namespace std;
using namespace sae::io;

struct vertex_data_type {
	bool isSeed;
	bool inside;
	bool changed;
	int alpha;
	int beta;

	vertex_data_type() {
		alpha = 0;
		beta = 0;
		isSeed = false;
		inside = false;
		changed = true;
	}

	vertex_data_type(int i) {
		alpha = 0;
		beta = 0;
		isSeed = true;
		inside = 1;
		changed = true;
	}
};

typedef float edge_data_type;

void test_create() {
    sae::io::GraphBuilder<int, vertex_data_type, edge_data_type> builder;


    builder.AddEdge(0, 10, float{10});
	builder.AddEdge(0, 20, float{10});
	builder.AddEdge(0, 30, float{10});
	builder.AddEdge(10, 40, float{10});

    builder.AddVertex(0, vertex_data_type(1));
    builder.AddVertex(10, vertex_data_type(1));
    builder.AddVertex(20, vertex_data_type(1));
    builder.AddVertex(30, vertex_data_type{});
	builder.AddVertex(40, vertex_data_type());


	builder.AddVertex(50, vertex_data_type{});

    builder.Save("alpha_beta_graph");
}

void test_load() {
    MappedGraph* g = MappedGraph::Open("alpha_beta_graph");
    cout << "loaded, n: " << g->VertexCount() << ", m:" << g->EdgeCount() << endl;

    for (auto vs = g->Vertices(); vs->Alive(); vs->Next()) {
        vertex_data_type* vd = (vertex_data_type*) vs->Data();
        cout << vs->Id() << ": " << vd->inside << endl;

        cout << "In Edges:" << endl;
        for (auto ei = vs->InEdges(); ei->Alive(); ei->Next()) {
            cout << "\t" << "[" << ei->SourceId() << "," << ei->TargetId() << "]" << ": " <<  endl;
        }

        cout << "Out Edges:" << endl;
        for (auto ei = vs->OutEdges(); ei->Alive(); ei->Next()) {
            cout << "\t" << "[" << ei->SourceId() << "," << ei->TargetId() << "]" << ": "  << endl;
        }
    }

    cout << endl;
    cout << "Forward Edges:" << endl;

    for (auto es = g->ForwardEdges(); es->Alive(); es->Next()) {
        edge_data_type* ed = (edge_data_type*) es->Data();
        cout << "\t" << es->Id() << "[" << es->Source()->Id() << "," << es->Target()->Id() << "]" << ": "  << endl;
    }

    cout << "Backward Edges:" << endl;
    for (auto es = g->BackwardEdges(); es->Alive(); es->Next()) {
        edge_data_type* ed = (edge_data_type*) es->Data();
        cout << "\t" << es->Id() << "[" << es->Source()->Id() << "," << es->Target()->Id() << "]" << ": "  << endl;
    }

    g->Close();
    delete g;
}

int main(int argc, const char * argv[]) {
    if (argc == 1 || (argc > 1 && argv[1][0] == 'c')) test_create();
    if (argc == 1 || (argc > 1 && argv[1][0] == 'l')) test_load();
}
