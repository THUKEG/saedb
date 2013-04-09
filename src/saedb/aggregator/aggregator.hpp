#ifndef SAE_AGGREGATOR
#define SAE_AGGREGATOR

namespace saedb{
//	template<typename ReductionType,
//			 typename VertexMapperType,
//			 typename EdgeMapperType,
//			 typename FinalizerType>
//	struct IAggregator{
//
//		//virtual void map()
//	};

	template<typename Graph, typename IContext>
	class AggregatorManager
	{
	public:
	    typedef Graph graph_type;
        typedef typename graph_type::vertex_type            vertex_type;
        typedef typename graph_type::edge_type              edge_type;
	    typedef IContext icontext_type;

	    graph_type& graph;
	    icontext_type* context;

	    /* annoyingly the mutable queue is a max heap when I need a min-heap
	     * to track the next thing to activate. So we need to keep
	     *  negative priorities... */
//	    mutable_queue<std::string, float> schedule;
//	    mutex schedule_lock;
	    size_t ncpus;
//	    std::map<std::string, IAggregator> aggregators;

	private:


	public:
	    AggregatorManager() {}

	    AggregatorManager(graph_type& graph,
				   icontext_type* context):
				  graph(graph),
	               context(context), ncpus(0) { }

	    void start(){

	    }

	    /**
	     * If synchronous aggregation is desired, this function is
	     * To be called simultaneously by one thread on each machine.
	     * This polls the schedule to see if there
	     * is an aggregator which needs to be activated. If there is an aggregator
	     * to be started, this function will perform aggregation.
	     */
	    void tick_synchronous(){
//	    	while(!schedule.empty() && schedule.top().second <= curtime){
//	    		std::string key = schedule.top().first;
//	    		aggregate
//	    	}
	    }

	    void stop(){

	    }

	    /**
	     * \copydoc graphlab::iengine::add_vertex_aggregator
	     */
//	    template <typename ReductionType,
//	              typename VertexMapperType,
//	              typename FinalizerType>
//	    bool add_vertex_aggregator(const std::string& key,
//	                               VertexMapperType map_function,
//	                               FinalizerType finalize_function) {
//	      if (key.length() == 0) return false;
//	      if (aggregators.count(key) == 0) {
//
//	        if (rmi.procid() == 0) {
//	          // do a runtime type check
//	          test_vertex_mapper_type<ReductionType, VertexMapperType>(key);
//	        }
//
//	        aggregators[key] = new map_reduce_type<ReductionType,
//	                                               VertexMapperType,
//	                                               typename default_map_types<ReductionType>::edge_map_type,
//	                                               FinalizerType>(map_function,
//	                                                             finalize_function);
//	        return true;
//	      }
//	      else {
//	        // aggregator already exists. fail
//	        return false;
//	      }
//	    }

//	    bool aggregate(const std::string& key){
//	    	//ASSERT_MSG(graph.is_finalized(), "Graph must be finalized");s
//	    	for(int i = 0; i < (int)graph.num_local_vertices(); ++i){
//	    		local_vertex_type lvertex = graph.l_vertex(i);
//	    		vertex_type vertex(lvertex);
//	    		local
//	    	}
//	    }

	    template <typename ResultType, typename MapFunctionType>
	    ResultType map_reduce_vertices(MapFunctionType mapfunction) {
	    	bool global_result_set = false;
	    	ResultType global_result = ResultType();

	    	//openmp
	    	bool result_set = false;
	    	ResultType result = ResultType();

	    	for (int i = 0; i < (int)graph.num_local_vertices(); ++i){
	    		if(!result_set){
	    			vertex_type vtx(graph.vertex(i));
	    			result = mapfunction(vtx);//(*context, vtx);
	    			result_set = true;
	    		}
	    		else if(result_set){
	    			vertex_type vtx(graph.vertex(i));
	    			result += mapfunction(vtx);//(*context, vtx);

	    		}
	    	}

	    	if(result_set){
	    		if(!global_result_set){
	    			global_result = result;
	    			global_result_set = true;
	    		}
	    		else{
	    			global_result += result;
	    		}
	    	}
	    }

	};
}

#endif
