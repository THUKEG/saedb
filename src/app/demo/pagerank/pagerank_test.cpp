#include <iostream>
#include <cmath>

#include "pagerank.hpp"
#include "test/testharness.hpp"
#include "gflags/gflags.h"

using namespace std;
using namespace sae::io;

class Empty {};

struct VData {
    double pagerank;
};

struct EData {
    int type;
};

DEFINE_bool(printall, true, "Print the values for all vertices.");

struct PageRankTest {
    string filepath;

    PageRankTest() {
        filepath = saedb::test::TempFileName();
        sae::io::GraphBuilder<int, VData, EData> b;
        b.AddEdge(0, 10, EData{0});
        b.AddEdge(10, 20, EData{0});
        b.AddEdge(20, 30, EData{0});
        b.AddVertex(0,  VData{1});
        b.AddVertex(10, VData{1});
        b.AddVertex(20, VData{1});
        b.AddVertex(30, VData{1});

        DataTypeAccessor* vd = b.CreateType("VData");
        std::cout << "Building type : " << vd->getTypeName() << std::endl;
        vd->appendField("pagerank", DOUBLE_T);
        b.SaveDataType(vd);

        DataTypeAccessor* ed = b.CreateType("EData");
        std::cout << "Building type : " << ed->getTypeName() << std::endl;
        ed->appendField("type", INT_T);
        b.SaveDataType(ed);

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
    if (FLAGS_printall) {
        for (auto i = 0; i < graph.num_local_vertices(); i ++) {
            cerr << "v[" << i << "]: " << graph.vertex(i).data() << endl;
        }
    }

    // compare with known answers
    vector<double> results = {0.15, 0.2775, 0.385875, 0.477994};
    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        ASSERT_TRUE(abs(results[i] - graph.vertex(i).data()) < TOLERANCE);
    }

    delete engine;
}

int main(int argc, char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    saedb::test::RunAllTests();
}

