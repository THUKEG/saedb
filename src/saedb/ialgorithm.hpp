#ifndef SAE_ALGORITHM_HPP
#define SAE_ALGORITHM_HPP

#include "graph_basic_types.hpp"
#include "icontext.hpp"

namespace saedb
{
    template <typename graph_t,
              typename gather_t,
              typename message_t = float>
    class IAlgorithm
    {
    public:
        typedef typename graph_t::vertex_data_type                vertex_data_type;
        typedef typename graph_t::edge_data_type                  edge_data_type;
        typedef gather_t                                          gather_type;
        typedef graph_t                                           graph_type;
        typedef message_t                                         message_type;
        typedef typename graph_type::vertex_id_type               vertex_id_type;
        typedef typename graph_type::vertex_type                  vertex_type;
        typedef typename graph_type::edge_type                    edge_type;
        typedef saedb::edge_dir_type                              edge_dir_type;
        typedef IContext<graph_type, gather_type, message_type>   icontext_type;

        // Initilize algorithm state
        virtual void
        init(icontext_type& context, vertex_type& vertex, const message_type& msg) { }

        // determine which type of edge to gather information
        virtual edge_dir_type
        gather_edges(icontext_type& context, const vertex_type& vertex) {
            return NO_EDGES;
        }

        // gather data on an edge
        virtual gather_type
        gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
            return gather_type();
        }

        // apply modification on a specific vertex
        virtual void
        apply(icontext_type& context, vertex_type& vertex, const gather_type& total) { }

        // determine which type of edge to scatter information
        virtual edge_dir_type
        scatter_edges(icontext_type& context, const vertex_type& vertex) const {
            return NO_EDGES;
        }

        // scatter data on an edge
        virtual void
        scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const { }

        virtual ~IAlgorithm() { }
    };
}
#endif
