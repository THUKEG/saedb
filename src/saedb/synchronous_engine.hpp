#ifndef SAE_SYNCHRONOUS_EGINE
#define SAE_SYNCHRONOUS_EGINE
#include <vector>
#include <iostream>
#include <bitset>
#include <mutex>

#include "engine.hpp"
#include "context.hpp"
#include "aggregator/aggregator.hpp"

namespace saedb
{
    template <typename VertexProgram>
    class sae_synchronous_engine :
    public IEngine<VertexProgram>
    {
    public:
	    typedef VertexProgram                               vertex_program_type;
	    typedef typename VertexProgram::gather_type         gather_type;
	    typedef typename VertexProgram::message_type        message_type;
	    typedef typename VertexProgram::vertex_data_type    vertex_data_type;
	    typedef typename VertexProgram::edge_data_type      edge_data_type;
	    typedef typename VertexProgram::graph_type          graph_type;
	    typedef typename graph_type::vertex_type            vertex_type;
	    typedef typename graph_type::edge_type              edge_type;
	    typedef typename graph_type::lvid_type              lvid_type;
        typedef context<sae_synchronous_engine>             context_type;
        
    private:
        /*
         * Friend class
         */
        friend class context<sae_synchronous_engine>;
        
    public:
	    sae_synchronous_engine(graph_type& graph);
        void signal_all();
	    void start();
        void RegisterAggregator(const string &, aggregator*);
        
    private:
	    void internal_stop();
        
	    template <typename MemberFunction>
	    void run_synchronous(MemberFunction func){
            ( (this)->*(func) )();
	    }
	    void execute_gathers();
	    void execute_applys();
	    void execute_scatters();
	    void execute_aggregate();
        
        // exchange messages signaled last iteration
        void receive_messages();
        
        void internal_signal(const vertex_type& vertex,
                             const message_type& message = message_type());
        aggregator* internalGetAggregator(const string& name);
        
        void clear_active_minorstep();
        void clear_active_superstep();
        void clear_messages();
        
        void count_active_vertices();
        
    private:
        graph_type& graph;
        size_t max_iterations;
        size_t iteration_counter;
        size_t num_active_vertices;
        std::vector<vertex_program_type>    vertex_programs;
        std::vector<gather_type>            gather_accum;
        std::vector<int>                    has_msg;
        std::vector<message_type>           messages;
        std::vector<int>                    active_superstep;
        std::vector<int>                    active_minorstep;
        std::map<string, aggregator*>       aggregators;
    };
    
    
    
    /*
     * Implementation of syn_engine
     **/
    template <typename VertexProgram>
    sae_synchronous_engine<VertexProgram>::sae_synchronous_engine(graph_type& graph):
    iteration_counter(0), max_iterations(10), graph(graph) {
        //	    vertex_programs.resize(graph.num_local_vertices());
	    gather_accum.resize(graph.num_local_vertices());
        //        local_vertex_lock.resize(graph.num_local_vertices());
        has_msg.resize(graph.num_local_vertices(), 0);
        messages.resize(graph.num_local_vertices(), message_type());
        active_superstep.resize(graph.num_local_vertices(), 0);
        active_minorstep.resize(graph.num_local_vertices(), 0);
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::start(){
	    std::cout << "Before running..." << std::endl;
	    graph.display();
	    while ( iteration_counter < max_iterations ){
            std::cout << "Iteration " << iteration_counter << std::endl;
            // mark vertex which has message as active in this superstep, no it is
            // not parallized
            receive_messages();
            clear_messages();
            count_active_vertices();
            std::cout << "num of active vertices: " << num_active_vertices << std::endl;
            
            run_synchronous( &sae_synchronous_engine::execute_gathers);
            run_synchronous( &sae_synchronous_engine::execute_applys);
            run_synchronous( &sae_synchronous_engine::execute_scatters);
            run_synchronous( &sae_synchronous_engine::execute_aggregate);
            aggregator* a = internalGetAggregator("max_pagerank");
            std::cout << "max pagerank" << *((float*)a->data()) << std::endl;
            ++iteration_counter;
	    }
        graph.display();
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::execute_gathers (){
	    // todo, how to get list of vertex ids to iterate?
	    context_type context(*this, graph);
	    auto vetex_ids = graph.vertex_ids;
        const vertex_program_type& vprog = vertex_program_type();
	    for(lvid_type vid : vetex_ids){
            if (!active_superstep[vid]) {
                continue;
            }
            // vertex is the same, hack
            //		  const vertex_program_type& vprog = vertex_programs[vid];
            vertex_type vertex(graph.vertex(vid));
            const edge_dir_type gather_dir = vprog.gather_edges(context, vertex);
            
            bool accum_is_set = false;
            gather_type accum = gather_type();
            if (gather_dir == IN_EDGES || gather_dir == ALL_EDGES){
                for(edge_type local_edge : vertex.in_edges()){
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
        const vertex_program_type& vprog = vertex_program_type();
	    for(lvid_type vid: vetex_ids){
            if (!active_superstep[vid]) {
                continue;
            }
            //		  const vertex_program_type& vprog = vertex_programs[vid];// no used here
            vertex_type vertex {graph.vertex(vid)};
            const edge_dir_type scatter_dir = vprog.scatter_edges(context, vertex);
            
            if (scatter_dir == IN_EDGES || scatter_dir == ALL_EDGES){
                for(edge_type local_edge : vertex.in_edges()){
                    edge_type edge(local_edge);
                    vprog.scatter(context, vertex, edge);
                }
            }
            
            if (scatter_dir == OUT_EDGES || scatter_dir == ALL_EDGES){
                for(edge_type local_edge : vertex.out_edges()){
                    edge_type edge(local_edge);
                    vprog.scatter(context, vertex, edge);
                }
            }
	    }
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    execute_applys (){
	    context_type context(*this, graph);
	    auto vetex_ids = graph.vertex_ids;
        vertex_program_type vprog = vertex_program_type();
	    for(lvid_type vid: vetex_ids){
            if (!active_superstep[vid]) {
                continue;
            }
            vertex_type vertex(graph.vertex(vid));
            const gather_type& accum = gather_accum[vid];
            vprog.apply(context, vertex, accum);
            // clear gather accum array
            gather_accum[vid] = gather_type();
	    }
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    execute_aggregate(){
	    context_type context(*this, graph);
        vertex_program_type *vprog = new vertex_program_type();
        lvid_type vid = 1;
        while (true) {
            if(vid > graph.num_local_vertices()){
                break;
            }
            vertex_type vertex(graph.vertex(vid));
            vprog->aggregate(context, vertex);
            vid++;
        }
        delete vprog;
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    receive_messages(){
        auto bit = active_superstep.begin();
        auto mit  = has_msg.begin();
        while( mit != has_msg.end() ){
            *bit = *mit;
            bit++;
            mit++;
        }
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    internal_signal(const vertex_type &vertex, const message_type& message){
        const lvid_type lvid = vertex.local_id();
        //        local_vertex_lock[lvid].lock();
        if (has_msg[lvid]) {
            messages[lvid] += message;
        }else{
            has_msg[lvid] = 1;
            messages[lvid] = message;
        }
        //        local_vertex_lock[lvid].unlock();
    }
    
    template <typename VertexProgram>
    aggregator* sae_synchronous_engine<VertexProgram>::
    internalGetAggregator(const string& name){
        return aggregators[name];
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    signal_all(){
        active_superstep.assign(graph.num_local_vertices(), 1);
        active_minorstep.assign(graph.num_local_vertices(), 1);
        has_msg.assign(graph.num_local_vertices(), 1);
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    clear_active_minorstep(){
        active_minorstep.assign(graph.num_local_vertices(), false);
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    clear_active_superstep(){
        active_superstep.assign(graph.num_local_vertices(), false);
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    clear_messages(){
        messages.assign(graph.num_local_vertices(), message_type());
        has_msg.assign(graph.num_local_vertices(), 0);
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    count_active_vertices(){
        num_active_vertices = 0;
        for(auto i : active_superstep){
            if(i){
                num_active_vertices++;
            }
        }
    }
    
    template <typename VertexProgram>
    void sae_synchronous_engine<VertexProgram>::
    RegisterAggregator(const string &name, aggregator* worker){
        aggregators[name] = worker;
    }
}
#endif