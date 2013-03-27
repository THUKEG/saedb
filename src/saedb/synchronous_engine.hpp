#ifndef SAE_SYNCHRONOUS_EGINE
#define SAE_SYNCHRONOUS_EGINE
#include <vector>
#include <iostream>
#include <bitset>
#include <mutex>

#include "iengine.hpp"
#include "context.hpp"
#include "aggregator/iaggregator.hpp"

namespace saedb
{
    template <typename VertexProgram>
    class SynchronousEngine:
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
        typedef Context<SynchronousEngine>                  context_type;
        
    private:
        /*
         * Friend class
         */
        friend class Context<SynchronousEngine>;
        
    public:
	    SynchronousEngine(graph_type& graph);
        void signalAll();
	    void start();
        void registerAggregator(const string &, IAggregator*);
        
    private:
	    void internalStop();
        
	    template <typename MemberFunction>
	    void runSynchronous(MemberFunction func){
            ( (this)->*(func) )();
	    }
	    void executeGathers();
	    void executeApplys();
	    void executeScatters();
	    void executeAggregate();
        
        // exchange messages signaled last iteration
        void receiveMessages();
        
        void internalSignal(const vertex_type& vertex,
                            const message_type& message = message_type());
        IAggregator* internalGetAggregator(const string& name);
        
        void clearActiveMinorstep();
        void clearActiveSuperstep();
        void clearMessages();
        
        void countActiveVertices();
        
    private:
        graph_type& graph_;
        size_t max_iterations_;
        size_t iteration_counter_;
        size_t num_active_vertices_;
        std::vector<vertex_program_type>    vertex_programs_;
        std::vector<gather_type>            gather_accum_;
        std::vector<int>                    has_msg_;
        std::vector<message_type>           messages_;
        std::vector<int>                    active_superstep_;
        std::vector<int>                    active_minorstep_;
        std::map<string, IAggregator*>      aggregators_;
    };
    
    
    
    /*
     * Implementation of syn_engine
     **/
    template <typename VertexProgram>
    SynchronousEngine<VertexProgram>::SynchronousEngine(graph_type& graph):
    iteration_counter_(0), max_iterations_(10), graph_(graph) {
        //	    vertex_programs_.resize(graph.num_local_vertices());
	    gather_accum_.resize(graph.num_local_vertices());
        //        local_vertex_lock.resize(graph.num_local_vertices());
        has_msg_.resize(graph.num_local_vertices(), 0);
        has_msg_.resize(graph.num_local_vertices(), message_type());
        active_superstep_.resize(graph.num_local_vertices(), 0);
        active_minorstep_.resize(graph.num_local_vertices(), 0);
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::start(){
	    std::cout << "Before running..." << std::endl;
	    graph_.display();
	    while ( iteration_counter_ < max_iterations_ ){
            std::cout << "Iteration " << iteration_counter_ << std::endl;
            // mark vertex which has message as active in this superstep, no it is
            // not parallized
            receiveMessages();
            clearMessages();
            countActiveVertices();
            std::cout << "num of active vertices: " << num_active_vertices_ << std::endl;
            
            runSynchronous( &SynchronousEngine::executeGathers);
            runSynchronous( &SynchronousEngine::executeApplys);
            runSynchronous( &SynchronousEngine::executeScatters);
            runSynchronous( &SynchronousEngine::executeAggregate);
            ++iteration_counter_;
	    }
        graph_.display();
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::executeGathers (){
	    // todo, how to get list of vertex ids to iterate?
	    context_type context(*this, graph_);
	    auto vetex_ids = graph_.vertex_ids;
        const vertex_program_type& vprog = vertex_program_type();
	    for(lvid_type vid : vetex_ids){
            if (!active_superstep_[vid]) {
                continue;
            }
            // vertex is the same, hack
            //		  const vertex_program_type& vprog = vertex_programs_[vid];
            vertex_type vertex(graph_.vertex(vid));
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
            gather_accum_[vid] = accum;
	    }
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::executeScatters (){
	    context_type context(*this, graph_);
	    auto vetex_ids = graph_.vertex_ids;
        const vertex_program_type& vprog = vertex_program_type();
	    for(lvid_type vid: vetex_ids){
            if (!active_superstep_[vid]) {
                continue;
            }
            //		  const vertex_program_type& vprog = vertex_programs_[vid];// no used here
            vertex_type vertex {graph_.vertex(vid)};
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
    void SynchronousEngine<VertexProgram>::
    executeApplys (){
	    context_type context(*this, graph_);
	    auto vetex_ids = graph_.vertex_ids;
        vertex_program_type vprog = vertex_program_type();
	    for(lvid_type vid: vetex_ids){
            if (!active_superstep_[vid]) {
                continue;
            }
            vertex_type vertex(graph_.vertex(vid));
            const gather_type& accum = gather_accum_[vid];
            vprog.apply(context, vertex, accum);
            // clear gather accum array
            gather_accum_[vid] = gather_type();
	    }
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::
    executeAggregate(){
	    context_type context(*this, graph_);
        vertex_program_type *vprog = new vertex_program_type();
        lvid_type vid = 1;
        while (true) {
            if(vid > graph_.num_local_vertices()){
                break;
            }
            vertex_type vertex(graph_.vertex(vid));
            vprog->aggregate(context, vertex);
            vid++;
        }
        delete vprog;
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::
    receiveMessages(){
        auto bit = active_superstep_.begin();
        auto mit  = has_msg_.begin();
        while( mit != has_msg_.end() ){
            *bit = *mit;
            bit++;
            mit++;
        }
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::
    internalSignal(const vertex_type &vertex, const message_type& message){
        const lvid_type lvid = vertex.local_id();
        //        local_vertex_lock[lvid].lock();
        if (has_msg_[lvid]) {
            messages_[lvid] += message;
        }else{
            has_msg_[lvid] = 1;
            messages_[lvid] = message;
        }
        //        local_vertex_lock[lvid].unlock();
    }
    
    template <typename VertexProgram>
    IAggregator* SynchronousEngine<VertexProgram>::
    internalGetAggregator(const string& name){
        return aggregators_[name];
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::
    signalAll(){
        active_superstep_.assign(graph_.num_local_vertices(), 1);
        active_minorstep_.assign(graph_.num_local_vertices(), 1);
        has_msg_.assign(graph_.num_local_vertices(), 1);
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::
    clearActiveMinorstep(){
        active_minorstep_.assign(graph_.num_local_vertices(), false);
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::
    clearActiveSuperstep(){
        active_superstep_.assign(graph_.num_local_vertices(), false);
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::
    clearMessages(){
        messages_.assign(graph_.num_local_vertices(), message_type());
        has_msg_.assign(graph_.num_local_vertices(), 0);
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::
    countActiveVertices(){
        num_active_vertices_ = 0;
        for(auto i : active_superstep_){
            if(i){
                num_active_vertices_++;
            }
        }
    }
    
    template <typename VertexProgram>
    void SynchronousEngine<VertexProgram>::
    registerAggregator(const string &name, IAggregator* worker){
        aggregators_[name] = worker;
    }
}
#endif