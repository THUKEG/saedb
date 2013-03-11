#ifndef SAE_GRAPH_HPP
#define SAE_GRAPH_HPP

#include <string>

#include "local_graph.hpp"

using namespace std;

/*
 * need doc
 * */
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
	    typedef saedb::sae_graph<VertexData, EdgeData> graph_type;
	    typedef saedb::vertex_id_type vertex_id_type;
	    typedef saedb::lvid_type lvid_type;
	    typedef saedb::edge_id_type edge_id_type;
    
	    struct vertex_type;
	    typedef bool edge_list_type;  
	    class edge_type;
	    
	    struct local_vertex_type;
	    struct local_edge_list_type;
	    class local_edge_type;

	    struct vertex_type {
		  graph_type& graph_ref;
		  lvid_type lvid;
		  
		  vertex_type(graph_type& graph_ref, lvid_type lvid):
			graph_ref(graph_ref), lvid(lvid) { }

		  bool operator==(vertex_type& v) const {
			return lvid == v.lvid;
		  }
      
		  const vertex_data_type& data() const {
			return graph_ref.get_local_graph().vertex_data(lvid);
		  }

		  vertex_data_type& data() {
		  }

		  size_t num_in_edges() const {
		  }

		  size_t num_out_edges() const {
		  }
      
		  vertex_id_type id() const {
			return graph_ref.global_vid(lvid);
		  }
 
		  edge_list_type in_edges() {
		  }

		  edge_list_type out_edges() {
		  }

		  lvid_type local_id() const {
			return lvid;
		  }
	    };

	    class edge_type {
	    private:
		  graph_type& graph_ref;
		  
		  typename local_graph_type::edge_type edge;

		  edge_type(graph_type& graph_ref,
			    typename local_graph_type::edge_type edge):
			graph_ref(graph_ref), edge(edge) { }
		  friend class sae_graph;
	    public:

		  vertex_type source() const { 
			return vertex_type(graph_ref, edge.source().id()); 
		  }

		  vertex_type target() const { 
			return vertex_type(graph_ref, edge.target().id()); 
		  }
      
		  const edge_data_type& data() const { return edge.data(); }
		  
		  edge_data_type& data() { return edge.data(); }
	    }; 

	    sae_graph() {/* nop */};

	    void finalize() { /* nop */}

	    size_t num_in_edges(const vertex_id_type vid) const {
	    }

	    size_t num_out_edges(const vertex_id_type vid) const {
	    }

	    void load() {
	    }

	    void loadformat ( string filename) {
	    }

	    void save() const {
	    }

	    int num_vertices() {
	    }

	    int num_edges() {
	    }

	    int num_local_vertices () {
	    }

	    vertex_type vertex(vertex_id_type vid){
	    }

      };

      struct local_vertex_type {
	    sae_graph& graph_ref;
	    lvid_type lvid;

	    local_vertex_type(sae_graph& graph_ref, lvid_type lvid):
		  graph_ref(graph_ref), lvid(lvid) { }

	    explicit local_vertex_type(vertex_type v) :graph_ref(v.graph_ref),lvid(v.lvid) { }

	    operator vertex_type() const {
		  return vertex_type(graph_ref, lvid);
	    }

	    bool operator==(local_vertex_type& v) const {
		  return lvid == v.lvid;
	    }
      
	    const vertex_data_type& data() const {
		  return graph_ref.get_local_graph().vertex_data(lvid);
	    }

	    vertex_data_type& data() {
		  return graph_ref.get_local_graph().vertex_data(lvid);
	    }

	    size_t num_in_edges() const {
		  return graph_ref.get_local_graph().num_in_edges(lvid);
	    }

	    size_t num_out_edges() const {
		  return graph_ref.get_local_graph().num_out_edges(lvid);
	    }

	    lvid_type id() const {
		  return lvid;
	    }

	    vertex_id_type global_id() const {
		  return graph_ref.global_vid(lvid);
	    }

	    local_edge_list_type in_edges() {
		  return graph_ref.l_in_edges(lvid);
	    }

	    local_edge_list_type out_edges() {
		  return graph_ref.l_out_edges(lvid);
	    }

	    size_t global_num_in_edges() const {
		  return graph_ref.l_get_vertex_record(lvid).num_in_edges;
	    }


	    size_t global_num_out_edges() const {
		  return graph_ref.l_get_vertex_record(lvid).num_out_edges;
	    }


	    vertex_record& get_vertex_record() {
		  return graph_ref.l_get_vertex_record(lvid);
	    }
      };

      class local_edge_type {
      private:
	    sae_graph& graph_ref;
	    typename local_graph_type::edge_type e;
      public:
	    local_edge_type(sae_graph& graph_ref,
			    typename local_graph_type::edge_type e):
		  graph_ref(graph_ref), e(e) { }
                      
	    explicit local_edge_type(edge_type ge) :graph_ref(ge.graph_ref),e(ge.e) { }

	    operator edge_type() const {
		  return edge_type(graph_ref, e);
	    }

	    local_vertex_type source() { return local_vertex_type(graph_ref, e.source().id()); }
      
	    local_vertex_type target() { return local_vertex_type(graph_ref, e.target().id()); }

	    const edge_data_type& data() const { return e.data(); }

	    edge_data_type& data() { return e.data(); }
      
	    edge_id_type id() const { return e.id(); }
      }; 

      struct local_edge_list_type {
	    // a list of edge in local graph, implement iterator using c++11
      }; 


}

#endif
