// Simple edge counting program
#include <iostream>
#include <random>
#include <algorithm>

#include "gflags/gflags.h"
#include "streaming/streaming.hpp"

using namespace std;
using namespace sae::streaming;

DEFINE_int32(threshold, 10, "wedge sample threshold; vertex with edges above this threshold will be counted by sampling.");
DEFINE_double(ratio, 0.3, "wedge sampling ratio");

struct WedgeSampler {
    eid_t triangles, wedges;
    std::vector<eid_t> edges;

    void init(const Context<WedgeSampler>& context, const Vertex& v) {}

    void edge(const Context<WedgeSampler>& context, vid_t id, const Edge& e) {
        if (e.source == id) {
            edges.push_back(e.target);
        } else if (e.target == id) {
            edges.push_back(e.source);
        }
    }

    void finalize() {
        // sort and unique edges
        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
    }

    void count(const Context<WedgeSampler>& context) {
        std::random_device rd;
        std::mt19937 gen(rd());

        wedges = edges.size() * (edges.size() - 1) / 2;
        triangles = 0;
        if (edges.size() <= FLAGS_threshold) {
            // direct
            for (int i = 0; i < edges.size(); i++) {
                auto& es = context.vertices[i].edges;
                std::vector<vid_t> intersection;
                std::set_intersection(es.begin(), es.end(), edges.begin(), edges.end(), std::back_inserter(intersection));
                triangles += intersection.size();
            }
            DLOG(INFO) << "Direct counting, triangles=" << triangles;
        } else {
            eid_t sample_size = wedges * FLAGS_ratio;
            eid_t tri = 0;
            std::uniform_int_distribution<int> dist(0, edges.size() - 1);
            for (eid_t i = 0; i < sample_size; i++) {
                vid_t s = dist(gen) , t = dist(gen);
                auto& es = context.vertices[edges[s]].edges;
                if (std::binary_search(es.begin(), es.end(), edges[t])) {
                    tri++;
                }
            }
            triangles = tri * wedges / sample_size;
            DLOG(INFO) << "Sampling, sample_size=" << sample_size << ", triangles=" << triangles;
        }
    }

    std::ostream& output(const Context<WedgeSampler>& context, vid_t id, std::ostream& os) const {
        os << triangles << " " << wedges;
        return os;
    }
};

int sgraph_main(StreamingGraph* g) {
    Context<WedgeSampler> context;
    SinglePassRun(context, g);
    context.run(&WedgeSampler::finalize);
    context.run(&WedgeSampler::count, context);
    context.output(cout);
    return 0;
}
