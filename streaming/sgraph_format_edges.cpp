#include <limits>
#include <string>
#include <iostream>
#include <algorithm>
#include "sgraph.hpp"


namespace sae {
namespace streaming {

struct Edges : public StreamingGraph {
    std::istream& is;

    Edges(std::istream& is) : is(is) {
    }

    void process(const Trigger<Graph>& onGraph, const Trigger<Vertex>& onVertex, const Trigger<Edge>& onEdge) {
        vid_t a, b;
        is >> a >> b;
        onGraph(Graph{a, b});
        for (vid_t i = 0; i < a; i++) {
            onVertex(Vertex{i, 0, ""});
        }
        eid_t eid = 0;
        while (is >> a >> b) {
            onEdge(Edge{eid, a, b, 0, ""});
            eid ++;
        }
    }
};

} // namespace streaming
} // namespace sae

REGSITER_GRAPH_FORMAT(edges, sae::streaming::Edges);
