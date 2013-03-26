#ifndef SAE_ENGINE_HPP
#define SAE_ENGINE_HPP

namespace saedb
{
    template <typename VertexProgramType>
    class sae_engine
    {
    public:
	    typedef VertexProgramType                           vertex_program_type;
	    typedef typename vertex_program_type::graph_type    graph_type;
	    typedef typename vertex_program_type::message_type  message_type;
	    typedef typename graph_type::vertex_id_type         vertex_id_type;
	    typedef typename graph_type::vertex_type            vertex_type;
	    typedef typename graph_type::edge_type              edge_type;
	    typedef typename vertex_program_type::icontext_type icontext_type;
	    
	    virtual ~sae_engine() {};
	    virtual void start() = 0;
	    virtual int iteration() const { return -1; }
    };
}
#endif