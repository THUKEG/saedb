#ifndef SAE_GRAPH_HPP
#define SAE_GRAPH_HPP

#include <string>

using namespace std;

namespace saedb
{
      template <typename VertexData,
		typename EdgeData>
      class sae_graph
      {
      public:
	    typedef VertexData vertex_data_type;
	    typedef EdgeData   edge_data_type;
	    typedef saedb::local_graph<VertexData, EdgeData> local_graph_type;
	    typedef saedb::distributed_graph<VertexData, EdgeData> graph_type;
	    typedef saedb::vertex_id_type vertex_id_type;
	    typedef saedb::lvid_type lvid_type;
	    typedef saedb::edge_id_type edge_id_type;
    
	    struct vertex_type;
	    typedef bool edge_list_type;  
	    class edge_type;
	    
	    struct local_vertex_type;
	    struct local_edge_list_type;
	    class local_edge_type;
}

#endif
