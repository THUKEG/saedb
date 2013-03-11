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
	    // Type members ===========================================================
	    /** The engine that created this context object */
	    typedef Engine engine_type;

	    /** The parent type */
	    typedef icontext<typename Engine::graph_type,
			     typename Engine::gather_type,
			     typename Engine::message_type> icontext_type;
	    typedef typename icontext_type::graph_type graph_type;
	    typedef typename icontext_type::vertex_id_type vertex_id_type;
	    typedef typename icontext_type::vertex_type vertex_type;   
	    typedef typename icontext_type::message_type message_type;
	    typedef typename icontext_type::gather_type gather_type;



      private:
	    /** A reference to the engine that created this context */
	    engine_type& engine;
	    /** A reference to the graph that is being operated on by the engine */
	    graph_type& graph;
       
      public:        
	    /** 
	     * \brief Construct a context for a particular engine and graph pair.
	     */
	    context(engine_type& engine, graph_type& graph) : 
		  engine(engine), graph(graph) { }

	    size_t num_vertices() const { return graph.num_vertices(); }

	    /**
	     * Get the number of edges in the graph
	     */
	    size_t num_edges() const { return graph.num_edges(); }

	    // /**
	    //  * Get an estimate of the number of update functions executed up
	    //  * to this point.
	    //  */
	    // size_t num_updates() const { return engine.num_updates(); }

	    size_t procid() const { return graph.procid(); }
      
	    size_t num_procs() const { return graph.numprocs(); }

	    std::ostream& cout() const {
		  return graph.dc().cout();
	    }

	    std::ostream& cerr() const {
		  return graph.dc().cerr();
	    }

	    /**
	     * Get the elapsed time in seconds
	     */
	    float elapsed_seconds() const { return engine.elapsed_seconds(); }

	    /**
	     * Return the current interation number (if supported).
	     */
	    int iteration() const { return engine.iteration(); }

	    /**
	     * Force the engine to stop executing additional update functions.
	     */
	    void stop() { engine.internal_stop(); }

	    /**
	     * Send a message to a vertex.
	     */
	    void signal(const vertex_type& vertex, 
			const message_type& message = message_type()) {
		  engine.internal_signal(vertex, message);
	    }

	    /**
	     * Send a message to a vertex ID.
	     * \warning This function will be slow since the current machine do
	     * not know the location of the vertex ID.
	     * \warning This may be unreliable. signals issued near to engine
	     * termination may be lost.
	     */
	    void signal_vid(vertex_id_type vid, 
			    const message_type& message = message_type()) {
		  engine.internal_signal_broadcast(vid, message);
	    }


	    /**
	     * Post a change to the cached sum for the vertex
	     */
	    void post_delta(const vertex_type& vertex, 
			    const gather_type& delta) {
		  engine.internal_post_delta(vertex, delta);
	    }

	    /**
	     * Invalidate the cached gather on the vertex.
	     */
	    virtual void clear_gather_cache(const vertex_type& vertex) { 
		  engine.internal_clear_gather_cache(vertex);      
	    }


                                                

      };
  
}

#endif

