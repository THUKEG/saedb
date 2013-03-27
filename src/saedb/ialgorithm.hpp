#ifndef SAE_ALGORITHM_HPP
#define SAE_ALGORITHM_HPP

#include "graph_basic_types.hpp"
#include "icontext.hpp"

namespace saedb
{
    template <typename Graph,
    typename GatherType,
    typename MessageType = float>
    class IAlgorithm
    {
    public:
	    typedef typename Graph::vertex_data_type                vertex_data_type;
	    typedef typename Graph::edge_data_type                  edge_data_type;
	    typedef GatherType                                      gather_type;
	    typedef Graph                                           graph_type;
	    typedef MessageType                                     message_type;
	    typedef typename graph_type::vertex_id_type             vertex_id_type;
	    typedef typename graph_type::vertex_type                vertex_type;
	    typedef typename graph_type::edge_type                  edge_type;
	    typedef saedb::edge_dir_type                            edge_dir_type;
	    typedef IContext<graph_type, gather_type, message_type> icontext_type;
	    
	    virtual ~IAlgorithm() { }
	    
	    virtual void init(icontext_type& context,
                          const vertex_type& vertex) { /** NOP */ }
        
	    virtual edge_dir_type gather_edges(icontext_type& context,
                                           const vertex_type& vertex) const {
            return IN_EDGES;
	    }
        
	    virtual gather_type gather(icontext_type& context,
                                   const vertex_type& vertex,
                                   edge_type& edge) const {
            return gather_type();
	    };
        
	    virtual void apply(icontext_type& context,
                           vertex_type& vertex,
                           const gather_type& total) = 0;
        
	    virtual edge_dir_type scatter_edges(icontext_type& context,
                                            const vertex_type& vertex) const {
            return OUT_EDGES;
	    }
        
	    virtual void scatter(icontext_type& context, const vertex_type& vertex,
                             edge_type& edge) const {
            std::cout << "Scatter not implemented" << std::endl;
	    };
        
        virtual void aggregate(icontext_type& context, const vertex_type& vertex){
            std::cout << "Aggregate not implemented" << std::endl;
        };
    };
}
#endif
