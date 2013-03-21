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
    MappedGraph* g = MappedGraph::Create("tg", 10, 20, sizeof(VData), sizeof(EData));

    auto vs = g->Vertices();
    for (; vs->Alive(); vs->Next()) {
        VData* vd = (VData*) vs->Data();
        vd->pagerank = vs->Id();
    }
    delete vs;

    auto es = g->Edges();
    for (; es->Alive(); es->Next()) {
        *es->MutableSource() = 1;
        *es->MutableTarget() = 2;
        *es->MutableDocId() = es->Id();
        EData* ed = (EData*) es->Data();
        ed->type = 5;
    }
    delete es;

    g->Close();
    delete g;
}

void test_load() {
    MappedGraph* g = MappedGraph::Open("tg");
    cout << "loaded, n: " << g->VertexCount() << ", m:" << g->EdgeCount() << endl;

    auto vs = g->Vertices();
    for (; vs->Alive(); vs->Next()) {
        VData* vd = (VData*) vs->Data();
        cout << vs->Id() << ": " << vd->pagerank << endl;
    }
    delete vs;

    auto es = g->Edges();
    for (; es->Alive(); es->Next()) {
        EData* ed = (EData*) es->Data();
        cout << es->Id() << "[" << es->Source() << "," << es->Target() << "]" << ": " << ed->type << endl;
    }
    delete es;

    g->Close();
    delete g;
}

int main() {
    test_create();
    test_load();
}
