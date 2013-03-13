#ifndef SAE_GRAPH_HPP
#define SAE_GRAPH_HPP

#include <string>
#include <iostream>
#include <map>
#include <set>
#include <vector>

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

	    typedef vector<edge_type> vertex_neighbours;
	    
	    set<vertex_id_type> vertex_ids;
	    map<vertex_id_type, vertex_type> vertex_id_2_vertex;
	    map<vertex_id_type, vertex_neighbours> vertex_id_2_in_edges;
	    map<vertex_id_type, vertex_neighbours> vertex_id_2_out_edges;
	    map<vertex_id_type, vertex_data_type> vertex_id_2_data;

	    size_t nvertex;

	    size_t nedge;

	    sae_graph() :
		  nvertex(0), nedge(0) {/* nop */};

	    void finalize() { /* nop */}

	    size_t num_in_edges(const vertex_id_type vid) const {
		  auto in_neighbours = vertex_id_2_in_edges[vid];
		  return in_neighbours.size();
	    }

	    size_t num_out_edges(const vertex_id_type vid) const {
		  auto out_neighbours = vertex_id_2_out_edges[vid];
		  return out_neighbours.size();
	    }

	    void load() {
	    }

	    void loadformat ( string filename) {
	    }

	    void save() const {
	    }

	    void display(){
		  typename map<vertex_id_type, vertex_data_type>::iterator it;
		  for ( it = vertex_id_2_data.begin(); it != vertex_id_2_data.end();++it){
			std::cout << it->first << " " << it->second << std::endl;
		  }
	    }

	    void add_vertex(vertex_id_type id, vertex_data_type data) {
		  std::set<vertex_id_type>::iterator it;
		  it = vertex_ids.find(id);
		  if ( it == vertex_ids.end() ){
			vertex_ids.insert(id);
			auto vertex = vertex_type(*this, id);
			vertex_id_2_vertex.insert( std::pair<vertex_id_type, vertex_type>(id, vertex) );
			vertex_id_2_data.insert( std::pair<vertex_id_type, vertex_data_type>(id, data) );
			++nvertex;
		  }
	    }

	    void add_edge(vertex_id_type source, vertex_id_type target, edge_data_type data) {
		  // now assum both source and target are inserted
		  vertex_neighbours out_nbr = vertex_id_2_out_edges[source];
		  vertex_neighbours in_nbr = vertex_id_2_out_edges[target];
		  out_nbr.push_back( edge_type(*this, source, target) );
		  in_nbr.push_back( edge_type(*this, source, target) );
		  ++nedge;
	    }

	    size_t num_vertices() {
		  return nvertex;
	    }

	    size_t num_edges() {
		  return nedge;
	    }

	    size_t num_local_vertices () {
		  return num_vertices();
	    }
	    
	    struct vertex_type {
		  sae_graph& graph_ref;
		  lvid_type lvid;
		  
		  vertex_type(sae_graph& graph_ref, lvid_type lvid):
			graph_ref(graph_ref), lvid(lvid) { }

		  vertex_type() {}

		  bool operator==(vertex_type& v) const {
			return lvid == v.lvid;
		  }
      
		  const vertex_data_type& data() const {
			return graph_ref.vertex_id_2_data[lvid];
		  }

		  vertex_data_type& data() {
			return graph_ref.vertex_id_2_data[lvid];
		  }

		  size_t num_in_edges() const {
		  }

		  size_t num_out_edges() const {
		  }
      
		  vertex_id_type id() const {
			return lvid;
		  }
 
		  vector<edge_type> in_edges() {
			return graph_ref.vertex_id_2_in_edges[lvid];
		  }

		  vector<edge_type> out_edges() {
			return graph_ref.vertex_id_2_out_edges[lvid];
		  }

		  lvid_type local_id() const {
			return lvid;
		  }
	    };

	    class edge_type {
	    private:
		  graph_type& graph_ref;
		  
 		  vertex_id_type source_id;

 		  vertex_id_type target_id;
		  
		  friend class sae_graph;
	    public:

		  edge_type(sae_graph& graph, vertex_id_type source, vertex_id_type target):
			graph_ref(graph), source_id(source), target_id(target){}

		  vertex_type source() const { 
			return vertex_type(graph_ref, source_id); 
		  }

		  vertex_type target() const { 
			return vertex_type(graph_ref, target_id); 
		  }
      
		  const edge_data_type& data() const {  }
		  
		  edge_data_type& data() { }
	    }; 


	    vertex_type vertex(vertex_id_type vid){
		  return vertex_type(*this, vid);
	    }


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
      };
}

#endif
