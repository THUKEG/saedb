#pragma once

#include "sae_include.hpp"

double RESET_PROB = 0.15;
double TOLERANCE = 1.0E-2;

typedef double vertex_data_type;
typedef double edge_data_type;
typedef float message_data_type;
typedef saedb::sae_graph graph_type;

class pagerank:
public saedb::IAlgorithm<graph_type, double, message_data_type>
{
public:
    void init(icontext_type& context, vertex_type& vertex, const message_type& msg) {
        //vertex.data() = msg;
    }

    edge_dir_type gather_edges(icontext_type& context,
                               const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    double gather(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
        return ((1.0 - RESET_PROB) / edge.source().num_out_edges()) * edge.source().parse<double>();
    }

    void apply(icontext_type& context, vertex_type& vertex,
               const gather_type& total){
        const double newval = total + RESET_PROB;
        vertex.update<double>(newval);
    }

    edge_dir_type scatter_edges(icontext_type& context,
                                const vertex_type& vertex) const{
        return saedb::OUT_EDGES;
    }

    void scatter(icontext_type& context, const vertex_type& vertex,
                 edge_type& edge) const {
        context.signal(edge.target());
    }

};
