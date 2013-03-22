#ifndef SAE_LOCALGRAPH_GPP
#define SAE_LOCALGRAPH_GPP

#include <vector>
#include "graph_basic_types.hpp"
#include "graph_storage.hpp"

namespace saedb {
      template<typename VertexData, typename EdgeData>
      class local_graph{
      public:
	    typedef graph_storage<VertexData, EdgeData> gstore_type;
	    typedef VertexData vertex_data_type;
	    
	    typedef EdgeData edge_data_type;

	    typedef typename gstore_type::edge_info edge_info;
	    
	    typedef saedb::vertex_id_type vertex_id_type;
	    typedef saedb::edge_id_type edge_id_type;

	    struct edge_type;
	    struct vertex_type;
	    struct edge_list_type;

	    struct edge_type {
		  local_graph& lgraph_ref;
		  typename gstore_type::edge_type e;
		  edge_type(local_graph& lgraph_ref, 
			    typename gstore_type::edge_type e) : 
			lgraph_ref(lgraph_ref),e(e) { }

		  const edge_data_type& data() const {
			return lgraph_ref.gstore.edge_data(e);
		  }

		  edge_data_type& data() {
			return lgraph_ref.gstore.edge_data(e);
		  }

		  vertex_type source() const {
			return vertex_type(lgraph_ref, e.source());
		  }

		  vertex_type target() const {
			return vertex_type(lgraph_ref, e.target());
		  }

		  edge_id_type id() const {
			return lgraph_ref.gstore.edge_id(e);
		  }
	    };

	    struct vertex_type {
		  local_graph& lgraph_ref;
		  lvid_type vid;
		  vertex_type(local_graph& lgraph_ref, lvid_type vid):lgraph_ref(lgraph_ref),vid(vid) { }
      
		  const vertex_data_type& data() const {
			return lgraph_ref.vertex_data(vid);
		  }
		  vertex_data_type& data() {
			return lgraph_ref.vertex_data(vid);
		  }

		  size_t num_in_edges() const {
			return lgraph_ref.num_in_edges(vid);
		  }

		  size_t num_out_edges() const {
			return lgraph_ref.num_out_edges(vid);
		  }

		  lvid_type id() const {
			return vid;
		  }

		  edge_list_type in_edges() {
			return edge_list_type(lgraph_ref, lgraph_ref.gstore.in_edges(vid));
		  }

		  edge_list_type out_edges() {
			return edge_list_type(lgraph_ref, lgraph_ref.gstore.out_edges(vid));
		  }
	    };

      };
	    
}
#endif
