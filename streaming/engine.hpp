#pragma once

#include <atomic>
#include <functional>
#include <iostream>
#include <thread>
#include <typeinfo>

#include "glog/logging.h"
#include "sgraph.hpp"

namespace sae {
namespace streaming {

// Aliasing placeholders for better readability.
extern decltype(std::placeholders::_1)& _vertex_id;
extern decltype(std::placeholders::_2)& _vertex_program;

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
    typedef Program UserProgram;

    int iteration;
    std::vector<Program> vertices;

    // Handy helper for running through all vertices.
    // The function will be binded to the vertex program with provided args.
    // You can optionally pass _vertex_id as an argument to receive the vertex id.
    // _vertex_program is used for referring to the vertex.
    template<typename M, typename... Args>
    void run(std::string job_name, M m, Args&&... args) {
        LOG(INFO) << "Started running " << job_name;
        auto func = std::bind(m, _vertex_program, std::forward<Args>(args)...);
        for (vid_t i = 0; i < vertices.size(); i++) {
            LOG_EVERY_N(INFO, vertices.size() / 100) << "Running " << job_name << " Progress: " << google::COUNTER << "/" << vertices.size();
            DLOG(INFO) << "Vertex " << i << " running " << job_name;
            func(i, vertices[i]);
        }
        LOG(INFO) << "Finished " << job_name;
    }

    // Exactly the same with `run`, except that it runs parallelly.
    // TODO a dynamic scheduler
    template<typename M, typename... Args>
    void run_parallel(std::string job_name, size_t threads, M m, Args&&... args) {
        if (threads > vertices.size()) {
            threads = vertices.size();
        }
        auto func = std::bind(m, _vertex_program, std::forward<Args>(args)...);
        LOG(INFO) << "Started running " << job_name << " with " << threads << " threads.";

        vid_t progress_interval = vertices.size() / 100;
        std::atomic<vid_t> counter;
        auto worker = [&](vid_t begin, vid_t end) {
            LOG(INFO) << "Worker started: " << begin << ", " << end;
            for (vid_t i = begin; i < end; i++) {
                if (counter.load() % progress_interval == 0) {
                    LOG(INFO) << "Running " << job_name << " Progress: " << counter.load() << "/" << vertices.size();
                }
                DLOG(INFO) << "Vertex " << i << " running " << job_name;
                func(i, vertices[i]);
                counter++;
            }
            LOG(INFO) << "Worker finished: " << begin << ", " << end;
        };

        std::vector<std::thread> pool;
        vid_t shard_size = vertices.size() / threads;
        for (size_t i = 0; i < threads; i++) {
            vid_t begin = shard_size * i;
            vid_t end = std::min(vid_t(shard_size * (i + 1)), vid_t(vertices.size()));
            pool.emplace_back(std::thread(worker, begin, end));
        }
        for (auto& t : pool) {
            t.join();
        }
        LOG(INFO) << "Finished " << job_name;
    }
};

template<class ProgramContext>
void SinglePassRun(ProgramContext& context, StreamingGraph* g) {
    std::string job_name = typeid(typename ProgramContext::UserProgram).name();
    LOG(INFO) << "Single Pass Runner for " << job_name << " started.";
    Graph graph;
    g->process([&](const Graph& g) {
        CHECK(g.n > 0) << "Vertices number must be positive.";
        LOG(INFO) << "Graph infomation: n=" << g.n << ", m=" << g.m;
        context.vertices.resize(g.n);
        graph = g;
    }, [&](const Vertex& v) {
        LOG_EVERY_N(INFO, graph.n / 100) << "Processing vertex " << v.id << ", Progress: " << google::COUNTER << "/" << graph.n;
        DLOG(INFO) << "Processing vertex: " << v.id;
        context.vertices[v.id].init(context, v);
    }, [&](const Edge& e) {
        LOG_EVERY_N(INFO, graph.m == 0 ? 100000 : graph.m / 100) << "Processing edge " << e.id << ", Progress: " << google::COUNTER << "/" << graph.m;
        DLOG(INFO) << "Processing edge: " << e.id;
        context.vertices[e.source].edge(context, e.source, e);
        context.vertices[e.target].edge(context, e.target, e);
    });

    LOG(INFO) << "Single Pass Runner for " << job_name << " finished.";
}

template<class ProgramContext>
void Output(ProgramContext& context, std::ostream& os) {
    context.run("output", &ProgramContext::UserProgram::output, std::ref(context), _vertex_id, std::ref(os));
}

}}
