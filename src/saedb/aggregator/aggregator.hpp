#ifndef SAE_AGGREGATOR
#define SAE_AGGREGATOR

namespace saedb{
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
        size_t ncpus;

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
        }

        void stop(){

        }

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

        template <typename ResultType, typename MapFunctionType>
        ResultType map_reduce_edges(MapFunctionType mapfunction){
            
            // TODO openmp
            bool result_set = false;
            ResultType result;

            for (int i=0; i<(int)graph.num_local_vertices(); i++){
                sae::io::EdgeIteratorPtr p = graph.vertex(i).out_edges();

                for (; p->Alive(); p->Next())
                {
                    sae::io::EdgeIteratorPtr q = p->Clone();
                    edge_type edge(std::move(q));

                    if (! result_set)
                    {
                        result_set = true;
                        result = mapfunction(std::move(edge));
                    }
                    else
                        result += mapfunction(std::move(edge));
                }
            }

            return result;
        }

    };
}

#endif
