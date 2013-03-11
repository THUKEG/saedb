#ifndef SAE_ICONTEX_HPP
#define SAE_ICONTEX_HPP

/**
 * not used now
 **/
namespace saedb {
      template<typename GraphType,
	       typename GatherType, 
	       typename MessageType>
      class icontext {
      public:
	    typedef GraphType graph_type;   

	    typedef typename graph_type::vertex_type vertex_type;

	    typedef typename graph_type::vertex_id_type vertex_id_type;

	    typedef MessageType message_type;

	    typedef GatherType gather_type;
   
      public:        
	    virtual ~icontext() { }
    
	    virtual size_t num_vertices() const { return 0; }

	    virtual size_t num_edges() const { return 0; }

	    virtual size_t procid() const { return 0; }

	    virtual std::ostream& cout() const { return std::cout; }

	    virtual std::ostream& cerr() const { return std::cerr; } 

	    virtual size_t num_procs() const { return 0; }

	    virtual float elapsed_seconds() const { return 0.0; }

	    virtual int iteration() const { return -1; } 

	    virtual void stop() { } 

	    virtual void signal(const vertex_type& vertex, 
                        const message_type& message = message_type()) { }

	    virtual void signal_vid(vertex_id_type gvid, 
                            const message_type& message = message_type()) { }

	    virtual void post_delta(const vertex_type& vertex, 
                            const gather_type& delta) { } 

	    virtual void clear_gather_cache(const vertex_type& vertex) { } 
      }; 
}
#endif

