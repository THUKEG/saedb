// A reader for CSV
// source,target,weight

#include <algorithm>
#include <limits>
#include <string>
#include <iostream>

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "sgraph.hpp"

DEFINE_int32(csv_n, 0, "number of vertices");

namespace sae {
namespace streaming {

struct CSV : public StreamingGraph {
    std::istream& is;

    CSV(std::istream& is) : is(is) {
    }

    void process(const Trigger<Graph>& onGraph, const Trigger<Vertex>& onVertex, const Trigger<Edge>& onEdge) {
        onGraph(Graph{FLAGS_csv_n, 0});
        for (vid_t i = 0; i < FLAGS_csv_n; i++) {
            onVertex(Vertex{i, 0, ""});
        }
        vid_t source, target;
        eid_t eid = 0;
        int weight;
        char c;
        while (is >> source >> c >> target >> c >> weight) {
            onEdge(Edge{eid, source, target, 0, std::to_string(weight)});
            eid++;
        }
    }
};

} // namespace streaming
} // namespace sae

REGSITER_GRAPH_FORMAT(csv, sae::streaming::CSV);
