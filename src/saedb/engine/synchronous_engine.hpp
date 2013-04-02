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
    template <typename algorithm_t>
    class SynchronousEngine:
    public IEngine<algorithm_t>
    {
    public:
		typedef algorithm_t                                 vertex_program_type;
		typedef typename algorithm_t::gather_type           gather_type;
		typedef typename algorithm_t::message_type          message_type;
		typedef typename algorithm_t::vertex_data_type      vertex_data_type;
		typedef typename algorithm_t::edge_data_type        edge_data_type;
		typedef typename algorithm_t::graph_type            graph_type;
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
		void start();
        void signalAll();
        void signalVertex(vertex_id_type vid);
        void signalVertices(const vector<vertex_id_type>&);
        void registerAggregator(const string &, IAggregator*);
        ~SynchronousEngine();
        
    private:
		void internalStop();
        
		template <typename MemberFunction>
		void runSynchronous(MemberFunction func){
            ( (this)->*(func) )();
		}
		void executeInits();
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
    template <typename algorithm_t>
    SynchronousEngine<algorithm_t>::SynchronousEngine(graph_type& graph):
    iteration_counter_(0), max_iterations_(10), graph_(graph) {
        vertex_programs_.resize(graph.num_local_vertices());
		gather_accum_.resize(graph.num_local_vertices());
        has_msg_.resize(graph.num_local_vertices(), 0);
        has_msg_.resize(graph.num_local_vertices(), message_type());
        active_superstep_.resize(graph.num_local_vertices(), 0);
        active_minorstep_.resize(graph.num_local_vertices(), 0);
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::start(){
		std::cout << "Before running..." << std::endl;
//		graph_.display();
		runSynchronous( &SynchronousEngine::executeInits);
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
//        graph_.display();
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::executeInits (){
		context_type context(*this, graph_);
		auto vertex_ids = graph_.vertex_ids;
		for(lvid_type vid : vertex_ids){
            if ( vid >= graph_.num_local_vertices() ){
                break;
            }
            auto &vprog = vertex_programs_[vid];
			vertex_type vertex(graph_.vertex(vid));
			vprog.init(context, vertex);
		}
    }

    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::executeGathers (){
		// todo, how to get list of vertex ids to iterate?
		context_type context(*this, graph_);
		auto vetex_ids = graph_.vertex_ids;
		for(lvid_type vid : vetex_ids){
            if (!active_superstep_[vid]) {
                continue;
            }
            auto &vprog = vertex_programs_[vid];
            vertex_type vertex(graph_.vertex(vid));
            auto gather_dir = vprog.gather_edges(context, vertex);
            
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
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::executeScatters (){
		context_type context(*this, graph_);
		auto vetex_ids = graph_.vertex_ids;
		for(lvid_type vid: vetex_ids){
            if (!active_superstep_[vid]) {
                continue;
            }
            auto &vprog = vertex_programs_[vid];
            vertex_type vertex {graph_.vertex(vid)};
            auto scatter_dir = vprog.scatter_edges(context, vertex);
            
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
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    executeApplys (){
		context_type context(*this, graph_);
		auto vetex_ids = graph_.vertex_ids;
		for(lvid_type vid: vetex_ids){
            if (!active_superstep_[vid]) {
                continue;
            }
            auto &vprog = vertex_programs_[vid];
            vertex_type vertex(graph_.vertex(vid));
            const auto& accum = gather_accum_[vid];
            vprog.apply(context, vertex, accum);
            // clear gather accum array
            gather_accum_[vid] = gather_type();
		}
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    executeAggregate(){
		context_type context(*this, graph_);
        lvid_type vid = 0;
        while (true) {
            if(vid >= graph_.num_local_vertices()){
                break;
            }
            auto &vprog = vertex_programs_[vid];
            vertex_type vertex(graph_.vertex(vid));
            vprog.aggregate(context, vertex);
            vid++;
        }
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    receiveMessages(){
        auto bit = active_superstep_.begin();
        auto mit  = has_msg_.begin();
        while( mit != has_msg_.end() ){
            *bit = *mit;
            bit++;
            mit++;
        }
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
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
    
    template <typename algorithm_t>
    IAggregator* SynchronousEngine<algorithm_t>::
    internalGetAggregator(const string& name){
        return aggregators_[name];
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    signalAll(){
        active_superstep_.assign(graph_.num_local_vertices(), 1);
        active_minorstep_.assign(graph_.num_local_vertices(), 1);
        has_msg_.assign(graph_.num_local_vertices(), 1);
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    signalVertex(vertex_id_type vid){
        active_superstep_[vid] = 1;
        active_minorstep_[vid] = 1;
        has_msg_[vid] = 1;
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    signalVertices(const vector<vertex_id_type>& vids){
        for (auto vid: vids) {
            signalVertex(vid);
        }
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    clearActiveMinorstep(){
        active_minorstep_.assign(graph_.num_local_vertices(), false);
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    clearActiveSuperstep(){
        active_superstep_.assign(graph_.num_local_vertices(), false);
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    clearMessages(){
        messages_.assign(graph_.num_local_vertices(), message_type());
        has_msg_.assign(graph_.num_local_vertices(), 0);
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    countActiveVertices(){
        num_active_vertices_ = 0;
        for(auto i : active_superstep_){
            if(i){
                num_active_vertices_++;
            }
        }
    }
    
    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    registerAggregator(const string &name, IAggregator* worker){
        aggregators_[name] = worker;
    }
    
    template <typename algorithm_t>
    SynchronousEngine<algorithm_t>::
    ~SynchronousEngine(){
        std::cout << "cleaning SynchonousEngine......" << std::endl;
    }
}
#endif
