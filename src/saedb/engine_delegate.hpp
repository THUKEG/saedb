#ifndef xcode_sae_EngineDelegate_hpp
#define xcode_sae_EngineDelegate_hpp
#include <vector>
#include <iostream>
#include <bitset>
#include <mutex>

#include "iengine.hpp"
#include "context.hpp"
#include "aggregator/iaggregator.hpp"

namespace saedb
{
    /*
     * This is the delegator for user to call related 
     * engine(now only SynchronousEngine).
     */
    template <typename VertexProgram>
    class EngineDelegate:
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
        typedef Context<EngineDelegate>                     context_type;
        
    private:
        /*
         * Friend class
         */
        friend class Context<EngineDelegate>;
        
    public:
        // TODO, add an option to select corresponding engine.
	    EngineDelegate(graph_type& graph);
        
        // mark all vertices as active
        void signalAll();
        
        // start engine
	    void start();
        
        // register related aggregator
        void registerAggregator(const string &, IAggregator*);
        
        ~EngineDelegate();
        
    private:
	    void internalStop();
        
        // exchange messages signaled last iteration
        void receiveMessages();
        
        void internalSignal(const vertex_type& vertex,
                            const message_type& message = message_type());
        IAggregator* internalGetAggregator(const string& name);
        
    private:
        // the real engine pointer
        IEngine<vertex_program_type> *engine;
    };
    
    
    
    /*
     * Implementation of EngineDelegate
     **/
    template <typename VertexProgram>
    EngineDelegate<VertexProgram>::EngineDelegate(graph_type& graph){
        // the default is SynchronousEngine
        // TODO here is where we select which engine we want, may need
        // an option.
        engine = new SynchronousEngine<VertexProgram>(graph);
    }
    
    template <typename VertexProgram>
    void EngineDelegate<VertexProgram>::start(){
        engine->start();
    }
    
    template <typename VertexProgram>
    void EngineDelegate<VertexProgram>::
    signalAll(){
        engine->signalAll();
    }
    
    template <typename VertexProgram>
    void EngineDelegate<VertexProgram>::
    registerAggregator(const string &name, IAggregator* worker){
        engine->registerAggregator(name, worker);
    }
    
    template <typename VertexProgram>
    EngineDelegate<VertexProgram>::
    ~EngineDelegate(){
        std::cout << "cleaning EngineDelegate" << std::endl;
        delete engine;
    }
}

#endif
