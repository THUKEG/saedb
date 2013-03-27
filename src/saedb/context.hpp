#ifndef SAE_CONTEXT_HPP
#define SAE_CONTEXT_HPP

#include <set>
#include <vector>
#include <cassert>

#include "icontext.hpp"

namespace saedb {
    template<typename Engine>
    class context :
    public icontext<typename Engine::graph_type,
                    typename Engine::gather_type,
                    typename Engine::message_type> {
    public:
	    typedef Engine                                  engine_type;
	    typedef icontext<typename Engine::graph_type,
                         typename Engine::gather_type,
                         typename Engine::message_type> icontext_type;
	    typedef typename icontext_type::graph_type      graph_type;
	    typedef typename icontext_type::vertex_id_type  vertex_id_type;
	    typedef typename icontext_type::vertex_type     vertex_type;
	    typedef typename icontext_type::message_type    message_type;
	    typedef typename icontext_type::gather_type     gather_type;
        
    private:
        // reference to engine
	    engine_type& engine;
        // reference to graph
	    graph_type& graph;
        
    public:
	    context(engine_type& engine, graph_type& graph) :
        engine(engine), graph(graph) { }
        
	    size_t num_vertices() const { return graph.num_vertices(); }
        
	    size_t num_edges() const { return graph.num_edges(); }
        
        size_t procid() const { return 0; }
        
        size_t num_procs() const {  return 0;}
                        
        // return the current iteration
        int iteration() const { return 0.0;}

        // force the engine to stop
	    void stop() { }
        
	    /**
	     * Send a message to a vertex.
	     */
	    void signal(const vertex_type& vertex,
                    const message_type& message = message_type()) {
            engine.internal_signal(vertex, message);
	    }
        
        /*
         * send a message to a vertex with specific id.
         */
        void signal_vid(vertex_id_type vid,
                        const message_type& message = message_type()) {
	    }
        
	    /**
	     * Post a change to the cached sum for the vertex
	     */
	    void post_delta(const vertex_type& vertex,
                        const gather_type& delta) {
	    }
                        
        aggregator* GetAggregator(const string& name) {
            return engine.internalGetAggregator(name);
        }
    };
}
#endif