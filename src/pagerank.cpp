#include <iostream>
#include <string>
#include "sae_include.hpp"

float RESET_PROB = 0.15;
float TOLERANCE = 1.0E-2;

typedef float vertex_data_type;
typedef float edge_data_type;
typedef saedb::empty message_date_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

class pagerank :
      public saedb::sae_algorithm<graph_type, float>
{
public:
      float gather(icontext_type& context, const vertex_type& vertex,
		   edge_type& edge) const {
	    return ((1.0 - RESET_PROB) / edge.source().num_out_edges()) *
		  edge.source().data();
      }
      
      void apply(icontext_type& context, vertex_type& vertex,
		 const gather_type& total){
	    const double newval = total + RESET_PROB;
	    last_change = std::fabs(newval - vertex.data());
	    vertex.data() = newval;
      }
      
      edge_dir_type scatter_edges(icontext_type& context,
				  const vertex_type& vertex) const{
	    if (last_change > TOLERANCE) return graphlab::OUT_EDGES;
	    else return graphlab::NO_EDGES;
      }
      
      void scatter(icontext_type& context, const vertex_type& vertex,
		   edge_type& edge) const {
	    context.signal(edge.target());
      }
};

int main()
{
      return 0;
}
