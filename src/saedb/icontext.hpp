#ifndef SAE_ICONTEX_HPP
#define SAE_ICONTEX_HPP

/*
 * Interface for communication between vertex programs.
 */
namespace saedb {
    template<typename graph_t,typename gather_t,typename message_t>
    class IContext
    {
    public:
        typedef graph_t                             graph_type;
        typedef typename graph_type::vertex_type    vertex_type;
        typedef typename graph_type::vertex_id_type vertex_id_type;
		typedef typename graph_type::edge_type		edge_type;
		typedef typename graph_type::edge_data_type edge_data_type;
        typedef message_t                           message_type;
        typedef gather_t                            gather_type;
        
    public:
        virtual size_t getNumVertices() const { return 0; }
        virtual size_t getNumEdges() const { return 0; }
        virtual size_t getProcid() const { return 0; }
        virtual size_t getNumProcs() const { return 0; }
        
        // return the current iteration
        virtual int getIteration() const {return -1;}
        
        // force the engine to stop
        virtual void stop() = 0;
        
        // activate vertex
        virtual void
        signal(const vertex_type& vertex, const message_type& message = message_type()) = 0;
        
        // active vertex with its id
        virtual void
        signalVid(vertex_id_type gvid, const message_type& message = message_type()) = 0;
        
        // get specific aggregator

		virtual void add_edge(vertex_id_type, vertex_id_type, edge_data_type) = 0;
        virtual IAggregator* getAggregator(const std::string&) = 0;
        
        virtual ~IContext() {}
    };
}
#endif
