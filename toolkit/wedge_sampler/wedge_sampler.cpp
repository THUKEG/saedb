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
DEFINE_double(sample_max, 50000, "maximum wedge samples");
DEFINE_int32(threads, 4, "parallel threads");

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
        thread_local std::random_device rd;
        thread_local std::mt19937 gen(rd());

        wedges = edges.size() * (edges.size() - 1) / 2;
        triangles = 0;
        if (FLAGS_threshold > 0 && edges.size() <= FLAGS_threshold) {
            // direct counting
            auto cur = edges.begin(), end = edges.end();
            while (cur != end) {
                auto& es = context.vertices[*cur++].edges;
                std::vector<vid_t> intersection;
                std::set_intersection(es.begin(), es.end(), cur, end, std::back_inserter(intersection));
                triangles += intersection.size();
            }
            DLOG(INFO) << "Direct counting, triangles=" << triangles;
        } else {
            // sampling
            eid_t sample_size = wedges * FLAGS_ratio;
            if (FLAGS_sample_max > 0 && sample_size > FLAGS_sample_max) {
                sample_size = FLAGS_sample_max;
            }
            eid_t tri = 0;
            std::uniform_int_distribution<int> dist(0, edges.size() - 1);
            for (int i = 0; i < sample_size; i++) {
                vid_t s = dist(gen) , t = dist(gen);
                if (s == t) {
                    sample_size--;
                    continue;
                }
                auto& es = context.vertices[edges[s]].edges;
                auto& et = context.vertices[edges[t]].edges;
                bool is_tri = es.size() < et.size() ? std::binary_search(es.begin(), es.end(), edges[t]) :
                                                      std::binary_search(et.begin(), et.end(), edges[s]);
                tri += is_tri ? 1 : 0;
            }
            triangles = sample_size > 0 ? tri * wedges / sample_size : 0;
            DLOG(INFO) << "Sampling, sample_size=" << sample_size << ", triangles=" << triangles;
        }
    }

    void output(const Context<WedgeSampler>& context, vid_t id, std::ostream& os) const {
        os << id << " " << triangles << " " << wedges << "\n";
    }
};

int sgraph_main(StreamingGraph* g) {
    LOG_IF(INFO, FLAGS_threshold <= 0) << "Not using sampling.";
    LOG_IF(INFO, FLAGS_sample_max <= 0) << "Not restricting sample_size.";
    Context<WedgeSampler> context;
    SinglePassRun(context, g);
    if (FLAGS_threads <= 1) {
        context.run("finalize", &WedgeSampler::finalize);
        context.run("count", &WedgeSampler::count, context);
    } else {
        context.run_parallel("finalize", FLAGS_threads, &WedgeSampler::finalize);
        context.run_parallel("count", FLAGS_threads, &WedgeSampler::count, context);
    }
    Output(context, cout);
    return 0;
}
