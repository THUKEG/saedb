#include <iostream>
#include <cmath>

#include "pagerank.hpp"
#include "test/testharness.hpp"

using namespace std;

class Empty {};

struct PageRankTest {
    string filepath;

    PageRankTest() {
        filepath = saedb::test::TempFileName();
        sae::io::GraphBuilder<int, double, Empty> b;
        b.AddEdge(0, 10, Empty());
        b.AddEdge(10, 20, Empty());
        b.AddEdge(20, 30, Empty());
        b.AddVertex(0, 1);
        b.AddVertex(10, 1);
        b.AddVertex(20, 1);
        b.AddVertex(30, 1);
        b.Save(filepath.c_str());
    }

    ~PageRankTest() {
        // TODO remove temp graph files
    }
};

TEST(PageRankTest, PageRank) {
    graph_type graph;
    graph.load_format(filepath);
    saedb::IEngine<pagerank> *engine = new saedb::EngineDelegate<pagerank>(graph);
    engine->signalAll();
    engine->start();

    // for debug purpose, print out all
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        cerr << "v[" << i << "]: " << graph.vertex(i).data() << endl;
    }

    // compare with known answers
    vector<double> results = {0.15, 0.2775, 0.385875, 0.477994};
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        ASSERT_TRUE(abs(results[i] - graph.vertex(i).data()) < TOLERANCE);
    }

    delete engine;
}

int main(){
    saedb::test::RunAllTests();
}

