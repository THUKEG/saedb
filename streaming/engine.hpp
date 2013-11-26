#pragma once

#include <iostream>
#include <functional>
#include <typeinfo>

#include "glog/logging.h"

namespace sae {
namespace streaming {

enum EdgeType {
    NO_EDGES = 0,
    IN_EDGES = 0x1,
    OUT_EDGES = 0x2,
    ALL_EDGES = IN_EDGES | OUT_EDGES
};

// Helper method for making functors behave like manipulators
inline std::ostream& operator<<(std::ostream& stream, const std::function<std::ostream& (std::ostream&)>& func) {
  return func(stream);
}

template<class Program>
struct Context {
    int iteration;
    std::vector<Program> vertices;

    // Handy helper for running through
    template <typename M, typename ...Args>
    void run(M m, Args&&... args) {
        for (vid_t i = 0; i < vertices.size(); i++) {
            DLOG(INFO) << "Vertex " << i << " running " << typeid(m).name();
            (vertices[i].*m)(std::forward<Args>(args)...);
        }
    }

    std::ostream& output(std::ostream& os) {
        for (vid_t i = 0; i < vertices.size(); i++) {
            os << i << "\t";
            vertices[i].output(*this, i, os);
            os << "\n";
        }
        return os;
    }
};

template<class Program>
void SinglePassRun(Context<Program>& context, StreamingGraph* g) {
    LOG(INFO) << "Single Pass Runner for " << typeid(Program).name();
    g->process([&](const Graph& g) {
        CHECK(g.n > 0) << "Vertices number must be positive.";
        LOG(INFO) << "Graph infomation: n=" << g.n << ", m=" << g.m;
        context.vertices.resize(g.n);
    }, [&](const Vertex& v) {
        DLOG(INFO) << "Processing vertex: " << v.id;
        context.vertices[v.id].init(context, v);
    }, [&](const Edge& e) {
        DLOG(INFO) << "Processing edge: " << e.id;
        context.vertices[e.source].edge(context, e.source, e);
        context.vertices[e.target].edge(context, e.target, e);
    });

    LOG(INFO) << "Single Pass Runner for " << typeid(Program).name() << " successfully finished.";
}

}}