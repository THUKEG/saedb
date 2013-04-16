#ifndef SAE_SYNCHRONOUS_EGINE
#define SAE_SYNCHRONOUS_EGINE
#include <vector>
#include <iostream>
#include <bitset>
#include <mutex>
#include <string>
#include <map>

#include "iengine.hpp"
#include "context.hpp"
#include "aggregator/aggregator.hpp"

#include "graph_basic_types.hpp"

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
        typedef Context<SynchronousEngine>                  context_type;
        typedef typename IEngine<algorithm_t>::aggregator_type aggregator_type;
        /**
         * \brief The distributed aggregator used to manage background
         * aggregation.
         */
        aggregator_type* aggregator;

    private:
        /*
         * Friend class
         */
        friend class Context<SynchronousEngine>;

    public:
        SynchronousEngine(graph_type& graph);
        void signalVertices(const std::vector<vertex_id_type>& vids);

        // signal a specific vertex with a message, update vertex data.
        void signalVertex(vertex_id_type vid, const message_type& msg);
        void signalAll();
        void start();
        ~SynchronousEngine();

        /**
         * \brief Get a pointer to the distributed aggregator object.
         *
         * This is currently used by the \ref graphlab::iengine interface to
         * implement the calls to aggregation.
         *
         * @return a pointer to the local aggregator.
         */
        aggregator_type* get_aggregator();

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
        // exchange messages signaled last iteration
        void receiveMessages();

        void internalSignal(const vertex_type& vertex,
                            const message_type& message = message_type());

        void internalSignal(const lvid_type& lvid,
                            const message_type& message = message_type());

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
        context_type* context;
    };



    /*
     * Implementation of syn_engine
     **/
    template <typename algorithm_t>
    SynchronousEngine<algorithm_t>::SynchronousEngine(graph_type& graph):
    iteration_counter_(0), max_iterations_(5), graph_(graph) {
        vertex_programs_.resize(graph.num_local_vertices());
        gather_accum_.resize(graph.num_local_vertices());
        has_msg_.resize(graph.num_local_vertices(), 0);
        messages_.resize(graph.num_local_vertices(), message_type());
        active_superstep_.resize(graph.num_local_vertices(), 0);
        active_minorstep_.resize(graph.num_local_vertices(), 0);
        context = new context_type(*this, graph);
        aggregator = new aggregator_type(graph, context);
    }


    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::start(){
        runSynchronous( &SynchronousEngine::executeInits);

        aggregator->start();
        while(true){
            // mark vertex which has message as active in this superstep, no it is
            // not parallized
            receiveMessages();
            clearMessages();
            countActiveVertices();
            if(num_active_vertices_ == 0){
                break;
            }
            runSynchronous( &SynchronousEngine::executeGathers);
            runSynchronous( &SynchronousEngine::executeApplys);
            runSynchronous( &SynchronousEngine::executeScatters);

            // probe the aggregator
            aggregator->tick_synchronous();
            ++iteration_counter_;
        }
    }

    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::executeInits (){
        context_type context(*this, graph_);
        lvid_type vid = 0;
        while (true) {
            if ( vid >= graph_.num_local_vertices() ){
                break;
            }
            auto &vprog = vertex_programs_[vid];
            vertex_type vertex = graph_.vertex(vid);
            vprog.init(context, vertex, messages_[vid]);
            vid++;
        }
    }

    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::executeGathers (){
        // todo, how to get list of vertex ids to iterate?
        context_type context(*this, graph_);
        for (lvid_type vid = 0; vid < graph_.num_local_vertices(); vid++) {
            if (!active_superstep_[vid]) {
                continue;
            }
            auto &vprog = vertex_programs_[vid];
            vertex_type vertex(graph_.vertex(vid));
            auto gather_dir = vprog.gather_edges(context, vertex);

            bool accum_is_set = false;
            gather_type accum = gather_type();
            if (gather_dir == IN_EDGES || gather_dir == ALL_EDGES){
                for (auto ep = vertex.in_edges(); ep->Alive(); ep->Next()) {
                    edge_type edge(ep->Clone());
                    if(accum_is_set) {
                        accum += vprog.gather(context, vertex, edge);
                    } else {
                        accum = vprog.gather(context, vertex, edge);
                        accum_is_set = true;
                    }
                }
            }

            if (gather_dir == OUT_EDGES || gather_dir == ALL_EDGES){
                for (auto ep = vertex.out_edges(); ep->Alive(); ep->Next()) {
                    edge_type edge(ep->Clone());
                    if(accum_is_set) {
                        accum += vprog.gather(context, vertex, edge);
                    } else {
                        accum = vprog.gather(context, vertex, edge);
                        accum_is_set = true;
                    }
                }
            }
            gather_accum_[vid] = accum;
            vid++;
        }
    }

    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::executeScatters (){
        context_type context(*this, graph_);
        for (lvid_type vid = 0; vid < graph_.num_local_vertices(); vid++) {
            if (!active_superstep_[vid]) {
                continue;
            }
            auto &vprog = vertex_programs_[vid];
            vertex_type vertex {graph_.vertex(vid)};
            auto scatter_dir = vprog.scatter_edges(context, vertex);

            if (scatter_dir == IN_EDGES || scatter_dir == ALL_EDGES){
                for (auto ep = vertex.in_edges(); ep->Alive(); ep->Next()) {
                    edge_type edge(ep->Clone());
                    vprog.scatter(context, vertex, edge);
                }
            }

            if (scatter_dir == OUT_EDGES || scatter_dir == ALL_EDGES){
                for (auto ep = vertex.out_edges(); ep->Alive(); ep->Next()) {
                    edge_type edge(ep->Clone());
                    vprog.scatter(context, vertex, edge);
                }
            }
            vid++;
        }
    }

    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    executeApplys (){
        context_type context(*this, graph_);
        for (lvid_type vid = 0; vid < graph_.num_local_vertices(); vid++) {
            if (!active_superstep_[vid]) {
                continue;
            }
            auto &vprog = vertex_programs_[vid];
            vertex_type vertex(graph_.vertex(vid));
            const auto& accum = gather_accum_[vid];
            vprog.apply(context, vertex, accum);
            // clear gather accum array
            gather_accum_[vid] = gather_type();
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
        lvid_type lvid = vertex.local_id();
        internalSignal(lvid, message);
    }

    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    internalSignal(const lvid_type &lvid, const message_type& message){
        if (has_msg_[lvid]) {
            messages_[lvid] += message;
        }else{
            has_msg_[lvid] = 1;
            messages_[lvid] = message;
        }
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
    signalVertex(vertex_id_type vid, const message_type& msg = message_type()){
        active_superstep_[vid] = 1;
        active_minorstep_[vid] = 1;
        has_msg_[vid] = 1;
        internalSignal(vid, msg);
    }

    template <typename algorithm_t>
    void SynchronousEngine<algorithm_t>::
    signalVertices(const std::vector<vertex_id_type>& vids){
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
    SynchronousEngine<algorithm_t>::
    ~SynchronousEngine(){
        delete aggregator;
        std::cout << "cleaning SynchonousEngine......" << std::endl;
    }


    template <typename algorithm_t>
    typename SynchronousEngine<algorithm_t>::aggregator_type*
    SynchronousEngine<algorithm_t>::get_aggregator(){
    	return aggregator;
    }
}
#endif
