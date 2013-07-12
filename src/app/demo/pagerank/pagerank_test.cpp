#include <iostream>
#include <cmath>

#include "pagerank.hpp"
#include "test/testharness.hpp"

using namespace std;
using namespace sae::io;

/*
 * serialization/deserialization for vertex data
 */

struct PageRankTest {
    string filepath;

    PageRankTest() {
        filepath = saedb::test::TempFileName();
        sae::io::GraphBuilder<int> b;
        b.AddVertexDataType("VData");
        b.AddEdgeDataType("EData");        

        b.AddVertex(0,  double(1.0), "VData");
        b.AddVertex(10, double(1.0), "VData");
        b.AddVertex(20, double(1.0), "VData");
        b.AddVertex(30, double(1.0), "VData");

        b.AddEdge(0, 10,  EData{0}, "EData");
        b.AddEdge(10, 20, EData{0}, "EData");
        b.AddEdge(20, 30, EData{0}, "EData");


        b.Save(filepath.c_str());
        cout << "pagerank_test:Generating graph data done. " << endl;
    }

    ~PageRankTest() {
        // TODO remove temp graph files
    }
};

TEST(PageRankTest, PageRank) {
    cout << "pagerank_test:[PageRank] Start test." << endl;
    graph_type graph;
    graph.load_format(filepath);
    saedb::IEngine<pagerank> *engine = new saedb::EngineDelegate<pagerank>(graph);
    engine->signalAll();
    engine->start();

    // for debug purpose, print out all
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        cout << "v[" << i << "]: " << graph.vertex(i).parse<double>() << endl;
    }

    // compare with known answers
    vector<double> results = {0.15, 0.2775, 0.385875, 0.477994};
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        ASSERT_TRUE(abs(results[i] - graph.vertex(i).parse<double>()) < TOLERANCE);
        ASSERT_TRUE(graph.vertex(i).data_type_name() == "VData");
    }
    cout << "pagerank_test:[PageRank] End test." << endl;
    delete engine;
}

int main(){
    saedb::test::RunAllTests();
}

