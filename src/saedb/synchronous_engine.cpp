#ifndef SAE_SYNCHRONOUS_EGINE
#define SAE_SYNCHRONOUS_EGINE
#include <vector>
#include <iostream>
#include "engine.hpp"
#include "context.hpp"

namespace saedb
{
      template <typename VertexProgram>
      class sae_synchronous_engine :
	    public sae_engine<VertexProgram>
      {
      public:
	    typedef VertexProgram vertex_program_type;
	    typedef typename VertexProgram::gather_type gather_type;
	    typedef typename VertexProgram::message_type message_type;
	    typedef typename VertexProgram::vertex_data_type vertex_data_type;
	    typedef typename VertexProgram::edge_data_type edge_data_type;
	    typedef typename VertexProgram::graph_type  graph_type;
	    typedef typename graph_type::vertex_type          vertex_type;
	    typedef typename graph_type::edge_type            edge_type;

	    typedef typename graph_type::lvid_type            lvid_type;
	    typedef context<sae_synchronous_engine> context_type;
	    friend class context<sae_synchronous_engine>;

      private:
	    graph_type& graph;
	    size_t max_iterations;
	    size_t iteration_counter;
	    std::vector<vertex_program_type> vertex_programs;
	    std::vector<gather_type>  gather_accum;

      public:
	    sae_synchronous_engine(graph_type& graph);
	    void start();

      private:
	    void internal_stop();

	    template <typename MemberFunction>
	    void run_synchronous(MemberFunction func){
		  ( (this)->*(func) )(); 
	    }
	    void execute_gathers();
	    void execute_applys();
	    void execute_scatters();
      }; //end of synchronos engine interface



      /*
       * Implementation of syn_engine
       **/
      template <typename VertexProgram>
      sae_synchronous_engine<VertexProgram>::sae_synchronous_engine(graph_type& graph):
      iteration_counter(0), max_iterations(10), graph(graph) {
	    vertex_programs.resize(graph.num_local_vertices());
	    gather_accum.resize(graph.num_local_vertices());
      }

      template <typename VertexProgram>
      void sae_synchronous_engine<VertexProgram>::start(){
	    std::cout << "Before running..." << std::endl;
	    graph.display();
	    while ( iteration_counter < max_iterations ){
		  std::cout << "Iteration " << iteration_counter << std::endl;
		  run_synchronous( &sae_synchronous_engine::execute_gathers);
		  run_synchronous( &sae_synchronous_engine::execute_applys);
		  run_synchronous( &sae_synchronous_engine::execute_scatters);
		  ++iteration_counter;
		  graph.display();
	    }
      }

      // tmp function
      template <typename to>
      void log(to t){
	    std::cout << t << std::endl;
      }
      
      template <typename VertexProgram>
      void sae_synchronous_engine<VertexProgram>::execute_gathers (){
	    // todo, how to get list of vertex ids to iterate?
	    context_type context(*this, graph);
	    auto vetex_ids = graph.vertex_ids;
	    for(lvid_type vid : vetex_ids){
		  // vertex is the same, hack
//		  const vertex_program_type& vprog = vertex_programs[vid];
		  const vertex_program_type& vprog = vertex_program_type();		  
		  vertex_type vertex(graph.vertex(vid));
		  const edge_dir_type gather_dir = vprog.gather_edges(context, vertex);

		  bool accum_is_set = false;
		  gather_type accum = gather_type();
		  if (gather_dir == IN_EDGES || gather_dir == ALL_EDGES){
			for(edge_type local_edge : vertex.in_edges()){
			      std::cout << "hi" << std::endl;
			      edge_type edge(local_edge);
			      if(accum_is_set) {
				    accum += vprog.gather(context, vertex, edge);
			      } else {
				    accum = vprog.gather(context, vertex, edge); 
				    accum_is_set = true;
			      }
			}
		  }
		  
		  if (gather_dir == OUT_EDGES || gather_dir == ALL_EDGES){
			for(edge_type local_edge : vertex.out_edges()){
			      edge_type edge(local_edge);
			      if(accum_is_set) {
				    accum += vprog.gather(context, vertex, edge);
			      } else {
				    accum = vprog.gather(context, vertex, edge); 
				    accum_is_set = true;
			      }
			}
		  }
		  gather_accum[vid] = accum;
	    }	    
      }
      
      template <typename VertexProgram>
      void sae_synchronous_engine<VertexProgram>::execute_scatters (){
	    context_type context(*this, graph);
	    auto vetex_ids = graph.vertex_ids;	    
	    for(lvid_type vid: vetex_ids){
//		  const vertex_program_type& vprog = vertex_programs[vid];// no used here
		  const vertex_program_type& vprog = vertex_program_type();		  		  
		  vertex_type vertex {graph.vertex(vid)};
		  const edge_dir_type gather_dir = vprog.scatter_edges(context, vertex);
		  
		  bool accum_is_set = false;
		  gather_type accum = gather_type();
		  if (gather_dir == IN_EDGES || gather_dir == ALL_EDGES){
			for(edge_type local_edge : vertex.in_edges()){
			      edge_type edge(local_edge);
			      vprog.scatter(context, vertex, edge);
			}
		  }
		  
		  if (gather_dir == OUT_EDGES || gather_dir == ALL_EDGES){
			for(edge_type local_edge : vertex.out_edges()){
			      edge_type edge(local_edge);
			      vprog.gather(context, vertex, edge);
			}
		  }
	    }	    
      }
      
      template <typename VertexProgram>
      void sae_synchronous_engine<VertexProgram>::execute_applys (){
	    context_type context(*this, graph);
	    auto vetex_ids = graph.vertex_ids;	    
	    for(lvid_type vid: vetex_ids){
		  vertex_type vertex(graph.vertex(vid));
		  const gather_type& accum = gather_accum[vid];
		  vertex_program_type vprog = vertex_program_type();
		  vprog.apply(context, vertex, accum);
		  // clear gather accum array
		  gather_accum[vid] = gather_type();
	    }
      }
}
#endif
