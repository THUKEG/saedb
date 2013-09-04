#ifndef xcode_sae_EngineDelegate_hpp
#define xcode_sae_EngineDelegate_hpp
#include <vector>
#include <iostream>
#include <bitset>
#include <mutex>

#include "synchronous_engine.hpp"
#include "iengine.hpp"

#include "../context.hpp"
#include "../aggregator/iaggregator.hpp"

namespace saedb
{
    /*
     * This is the delegator for user to call related
     * engine(now only SynchronousEngine).
     */
    template <typename algorithm_t>
    class EngineDelegate:
    public IEngine<algorithm_t>
    {
    public:
        typedef algorithm_t                                 vertex_program_type;
        typedef typename algorithm_t::gather_type           gather_type;
        typedef typename algorithm_t::message_type          message_type;
        typedef typename algorithm_t::graph_type            graph_type;
        typedef typename graph_type::vertex_type            vertex_type;
        typedef typename graph_type::edge_type              edge_type;
        typedef Context<EngineDelegate>                     context_type;
        /**
         * \brief The type of the distributed aggregator used by each engine to
         * implement distributed aggregation.
         */
        typedef typename IEngine<algorithm_t>::aggregator_type aggregator_type;

    public:
        // TODO, add an option to select corresponding engine.
        EngineDelegate(graph_type& graph);

        // mark all vertices as active
        void signalAll();

        void signalVertex(vertex_id_type, const message_type&);

        void signalVertices(const std::vector<vertex_id_type>&);

        // start engine
        void start();

        /**
         * \brief Get a pointer to the distributed aggregator object.
         *
         * This is currently used by the \ref graphlab::iengine interface to
         * implement the calls to aggregation.
         *
         * @return a pointer to the local aggregator.
         */
        aggregator_type* get_aggregator();

        ~EngineDelegate();

    private:
        /*
         * Friend class
         */
        friend class Context<EngineDelegate>;

    private:
        void internalStop();

        // exchange messages signaled last iteration
        void receiveMessages();

        void internalSignal(const vertex_type& vertex,
                            const message_type& message = message_type());

    private:
        // the real engine pointer
        IEngine<vertex_program_type> *engine;
    };



    /*
     * Implementation of EngineDelegate
     **/
    template <typename algorithm_t>
    EngineDelegate<algorithm_t>::EngineDelegate(graph_type& graph){
        // the default is SynchronousEngine
        // TODO here is where we select which engine we want, may need
        // an option.
        engine = new SynchronousEngine<algorithm_t>(graph);
    }

    template <typename algorithm_t>
    void EngineDelegate<algorithm_t>::start(){
        engine->start();
    }

    template <typename algorithm_t>
    void EngineDelegate<algorithm_t>::
    signalAll(){
        engine->signalAll();
    }

    template <typename algorithm_t>
    void EngineDelegate<algorithm_t>::
    signalVertex(vertex_id_type vid, const message_type& msg = message_type()){
        engine->signalVertex(vid, msg);
    }

    template <typename algorithm_t>
    void EngineDelegate<algorithm_t>::
    signalVertices(const std::vector<vertex_id_type> & vids){
        engine->signalVertices(vids);
    }

    template<typename algorithm_t>
    typename EngineDelegate<algorithm_t>::aggregator_type*
    EngineDelegate<algorithm_t>::get_aggregator() {
      return engine->get_aggregator();
    } // end of get_aggregator

    template <typename algorithm_t>
    EngineDelegate<algorithm_t>::
    ~EngineDelegate(){
        std::cout << "cleaning EngineDelegate" << std::endl;
        delete engine;
    }
}

#endif
