#ifndef SAE_CONTEXT_HPP
#define SAE_CONTEXT_HPP

#include <set>
#include <vector>
#include <cassert>
#include <string>

#include "icontext.hpp"

namespace saedb {
    template<typename Engine>
    class Context:
    public IContext<typename Engine::graph_type,
    typename Engine::gather_type,
    typename Engine::message_type> {
    public:
        typedef Engine                                  engine_type;
        typedef IContext<typename Engine::graph_type,
        typename Engine::gather_type,
        typename Engine::message_type> icontext_type;
        typedef typename icontext_type::graph_type      graph_type;
        typedef typename icontext_type::vertex_id_type  vertex_id_type;
        typedef typename icontext_type::vertex_type     vertex_type;
        typedef typename icontext_type::message_type    message_type;
        typedef typename icontext_type::gather_type     gather_type;

    private:
        // reference to engine
        engine_type& engine_;
        // reference to graph
        graph_type& graph_;

    public:
        Context(engine_type& engine, graph_type& graph):
        engine_(engine), graph_(graph) { }

        size_t getNumVertices() const { return graph_.num_vertices(); }

        size_t getNumEdges() const { return graph_.num_edges(); }

        size_t getProcid() const { return 0; }

        size_t getNumProcs() const {  return 0;}

        // return the current iteration
        int getIteration() const { return 0.0;}

        // force the engine to stop
        void stop() { }

        /**
         * Send a message to a vertex.
         */
        void signal(const vertex_type& vertex,
                    const message_type& message = message_type()) {
            engine_.internalSignal(vertex, message);
        }

        /*
         * send a message to a vertex with specific id.
         */
        void signalVid(vertex_id_type vid,
                       const message_type& message = message_type()) {
        }

    };
}
#endif
