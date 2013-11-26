// The Steraming Graph Interface

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <map>

#include "trigger.hpp"

namespace sae {
namespace streaming {

using vid_t = int64_t;
using eid_t = int64_t;
using tid_t = uint8_t;
using data_t = std::string;

struct Graph {
    vid_t n;
    vid_t m;
};

struct Vertex {
    vid_t id;
    tid_t type;
    data_t data;

    bool operator<(const Vertex& v) const {
        return id < v.id;
    }

    bool operator==(const Vertex& v) const {
        return id == v.id;
    }
};

struct Edge {
    eid_t id;
    vid_t source;
    vid_t target;
    tid_t type;
    data_t data;

    bool operator<(const Edge& e) const {
        return id < e.id;
    }

    bool operator==(const Edge& e) const {
        return id == e.id;
    }
};

// Implementations should guarantee that the callbacks are in the specific order: graph, vertex, edge.
struct StreamingGraph {
    virtual ~StreamingGraph() {};
    virtual void process(const Trigger<Graph>&,
                         const Trigger<Vertex>&,
                         const Trigger<Edge>&) = 0;
};

// The following code are for automatic graph format registering
using StreamingGraphCreator = std::function<StreamingGraph*(std::istream&)>;
using GraphFormatMap = std::map<std::string, StreamingGraphCreator>;
extern GraphFormatMap *graph_format_map;

struct GraphFormatRegisterer {
    GraphFormatRegisterer(const char *, StreamingGraphCreator&&);
};

} // namespace streaming
} // namespace sae

#define REGSITER_GRAPH_FORMAT(name, klass) \
    namespace sae { namespace streaming { \
        GraphFormatRegisterer graph_format_registerer_##name(#name, [](std::istream& is) { \
            return new klass(is); \
        }); \
    }}
