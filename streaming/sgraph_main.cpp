// This is a universal main for sgraph.

#include <iostream>
#include <string>
#include <fstream>
#include "streaming.hpp"

using namespace std;
using namespace sae::streaming;

DEFINE_string(graph, "", "graph file");
DEFINE_string(format, "csv", "graph format");

extern int sgraph_main(StreamingGraph*);

int main(int argc, char * argv[]) {
    if (graph_format_map == nullptr) {
        LOG(FATAL) << "No graph reader registered.";
    } else {
        string supported_format = "Supported graph format:";
        for (auto& kv : (*graph_format_map)) {
            supported_format += " " + kv.first;
        }
        gflags::SetUsageMessage(supported_format.c_str());
    }

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InstallFailureSignalHandler();
    google::InitGoogleLogging(argv[0]);

    LOG(INFO) << "Finding graph format reader for: " << FLAGS_format;
    std::unique_ptr<StreamingGraph> g;
    auto graph_creator = graph_format_map->find(FLAGS_format);
    if (graph_creator == graph_format_map->end()) {
        LOG(FATAL) << "Format not supported: " << FLAGS_format;
    }

    LOG(INFO) << "Openning graph file: " << FLAGS_graph;
    std::ifstream is(FLAGS_graph);
    if (!is) {
        LOG(FATAL) << "Cannot open graph file: " << FLAGS_graph;
    }
    g.reset(graph_creator->second(is));

    LOG(INFO) << "Transferring to user program";
    return sgraph_main(g.get());
}
