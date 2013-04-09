#ifndef SAE_ENGINE_HPP
#define SAE_ENGINE_HPP
#include <string>
#include "aggregator/aggregator.hpp"

namespace saedb
{
    template <typename algorithm_t>
    class IEngine
    {
    public:
        typedef algorithm_t                                 vertex_program_type;
        typedef typename vertex_program_type::graph_type    graph_type;
        typedef typename vertex_program_type::message_type  message_type;
        typedef typename graph_type::vertex_id_type         vertex_id_type;
        typedef typename graph_type::vertex_type            vertex_type;
        typedef typename graph_type::edge_type              edge_type;
        typedef typename vertex_program_type::icontext_type icontext_type;

        /**
         * \brief The type of the distributed aggregator used by each engine to
         * implement distributed aggregation.
         */
        typedef AggregatorManager<graph_type, icontext_type> aggregator_type;

        // start engine
        virtual void start() = 0;

        // mark all vertices as active
        virtual void signalAll() = 0;
        
        // mark a vertex as active
        virtual void signalVertex(vertex_id_type) = 0;
        
        // mark some vertices as active
        virtual void signalVertices(const std::vector<vertex_id_type>&) = 0;

        /**
         * \brief Performs a map-reduce operation on each vertex in the
         * graph returning the result.
         *
         * Given a map function, map_reduce_vertices() call the map function on all
         * vertices in the graph. The return values are then summed together and the
         * final result returned. The map function should only read the vertex data
         * and should not make any modifications. map_reduce_vertices() must be
         * called on all machines simultaneously.
         *
         * ### Basic Usage
         * For instance, if the graph has float vertex data, and float edge data:
         * \code
         *   typedef graphlab::distributed_graph<float, float> graph_type;
         * \endcode
         *
         * To compute an absolute sum over all the vertex data, we would write
         * a function which reads in each a vertex, and returns the absolute
         * value of the data on the vertex.
         * \code
         * float absolute_vertex_data(engine_type::icontext_type& context,
         *                            graph_type::vertex_type vertex) {
         *   return std::fabs(vertex.data());
         * }
         * \endcode
         * After which calling:
         * \code
         * float sum = engine.map_reduce_vertices<float>(absolute_vertex_data);
         * \endcode
         * will call the <code>absolute_vertex_data()</code> function
         * on each vertex in the graph. <code>absolute_vertex_data()</code>
         * reads the value of the vertex and returns the absolute result.
         * This return values are then summed together and returned.
         * All machines see the same result.
         *
         * The template argument <code><float></code> is needed to inform
         * the compiler regarding the return type of the mapfunction.
         *
         * ### Signalling
         * Another common use for the map_reduce_vertices() function is
         * in signalling. Since the map function is passed a context, it
         * can be used to perform signalling of vertices for execution
         * during a later \ref start() "engine.start()" call.
         *
         * For instance, the following code will signal all vertices
         * with value >= 1
         * \code
         * graphlab::empty signal_vertices(engine_type::icontext_type& context,
         *                                 graph_type::vertex_type vertex) {
         *   if (vertex.data() >= 1) context.signal(vertex);
         *   return graphlab::empty()
         * }
         * \endcode
         * Note that in this case, we are not interested in a reduction
         * operation, and thus we return a graphlab::empty object.
         * Calling:
         * \code
         * engine.map_reduce_vertices<graphlab::empty>(signal_vertices);
         * \endcode
         * will run <code>signal_vertices()</code> on all vertices,
         * signalling all vertices with value <= 1
         *
         * ### Relations
         * The map function has the same structure as that in
         * add_vertex_aggregator() and may be reused in an aggregator.
         * This function is also very similar to
         * graphlab::distributed_graph::map_reduce_vertices()
         * with the difference that this takes a context and thus
         * can be used to perform signalling.
         * Finally transform_vertices() can be used to perform a similar
         * but may also make modifications to graph data.
         *
         * \tparam ReductionType The output of the map function. Must have
         *                    operator+= defined, and must be \ref sec_serializable.
         * \tparam VertexMapperType The type of the map function.
         *                          Not generally needed.
         *                          Can be inferred by the compiler.
         * \param mapfunction The map function to use. Must take an
         *                   \ref icontext_type& as its first argument, and
         *                   a \ref vertex_type, or a reference to a
         *                   \ref vertex_type as its second argument.
         *                   Returns a ReductionType which must be summable
         *                   and \ref sec_serializable .
         */
         template <typename ReductionType, typename VertexMapperType>
         ReductionType map_reduce_vertices(VertexMapperType mapfunction) {
           aggregator_type* aggregator = get_aggregator();
//           BOOST_CONCEPT_ASSERT((graphlab::Serializable<ReductionType>));
//           BOOST_CONCEPT_ASSERT((graphlab::OpPlusEq<ReductionType>));

           if(aggregator == NULL) {
//             logstream(LOG_FATAL) << "Aggregation not supported by this engine!"
//                                  << std::endl;
             return ReductionType(); // does not return
           }
           return aggregator->template map_reduce_vertices<ReductionType>(mapfunction);
         }


        /**
         * \brief An overload of add_edge_aggregator for C++11 which does not
         *        require the user to provide the reduction type.
         *
         * This function is available only if the compiler has C++11 support.
         * Specifically, it uses C++11's decltype operation to infer the
         * reduction type, thus eliminating the need for the function
         * call to be templatized over the reduction type. For instance,
         * in the add_edge_aggregator() example, it allows the following
         * code to be written:
         * \code
         * engine.add_edge_aggregator("absolute_edge_sum",
         *                              absolute_edge_data,
         *                              print_finalize);
         * \endcode
         *
         * \tparam EdgeMapperType The type of the map function.
         *                          Not generally needed.
         *                          Can be inferred by the compiler.
         * \tparam FinalizerType The type of the finalize function.
         *                       Not generally needed.
         *                       Can be inferred by the compiler.
         *
         * \param [in] key The name of this aggregator. Must be unique.
         * \param [in] map_function The Map function to use. Must take an
         *                          \ref icontext_type& as its first argument, and
         *                          a \ref vertex_type, or a reference to a
         *                          \ref vertex_type as its second argument.
         *                          Returns a ReductionType which must be summable
         *                          and \ref sec_serializable .
         * \param [in] finalize_function The Finalize function to use. Must take
         *                               an \ref icontext_type& as its first
         *                               argument and a ReductionType, or a
         *                               reference to a ReductionType as its second
         *                               argument.
         */
//        template<typename VertexMapType,
//        		 typename FinalizerType>
//        bool add_vertex_aggregator(const std::string& key,
//        						   VertexMapType map_function,
//        						   FinalizerType finalize_function){
//        	aggregator_type* aggregator = get_aggregator();
////        	if(aggregator == NULL){
////        		logstream
////        	}
//        	return aggregator->add_vertex_aggregator(key, map_function, finalize_function);
//        }

        virtual aggregator_type* get_aggregator() = 0;

        virtual ~IEngine() {}
    };
}
#endif
