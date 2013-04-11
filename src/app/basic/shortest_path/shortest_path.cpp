#include <iostream>
#include <limits>
#include <algorithm>
#include "sae_include.hpp"

using namespace saedb;

#define MAXFL (numeric_limits<float>::max())

typedef sae_graph<float, float> G;

struct SP_dis {
    float dis;
    SP_dis(float dis_ = MAXFL) : dis(dis_) {}
    SP_dis& operator +=(const SP_dis& other)
    {
        dis = min(dis, other.dis);
        return *this;
    }
};

class shortest_path: public IAlgorithm<G, SP_dis, SP_dis>
{
public:

    void init(icontext_type& context, vertex_type& vertex, const message_type& msg) const {
        vertex.data() = msg.dis;
    }

    edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const {
        return IN_EDGES;
    }

    gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
    {
        float newval = edge.data() + edge.source().data();
        return newval;
    }

    void apply(icontext_type& context, vertex_type& vertex, const gather_type& total)
    {
        vertex.data() = min(vertex.data(), total.dis);
    }

    edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
    {
        return OUT_EDGES;
    }

    void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
    {
        if (vertex.data() + edge.data() < edge.target().data())
            context.signal(edge.target());
    }
};

void gen_test_graph(char const * path) {
    sae::io::GraphBuilder<int, float, float> builder;

    builder.AddEdge(0, 10, 10);
    builder.AddEdge(0, 20, 40);
    builder.AddEdge(10, 20, 20);
    builder.AddEdge(20, 30, 30);
    builder.AddEdge(30, 40, 40);

    builder.Save(path);
}

int main()
{
    const char * graph_path = "shortest_path_graph";
    gen_test_graph(graph_path);

    G graph;
    graph.load_format(graph_path);

    const auto seed = 0;

    IEngine<shortest_path> *engine = new EngineDelegate<shortest_path>(graph);
    engine->signalVertex(seed, 0);
    engine->start();

    for (auto i = 0; i < graph.num_local_vertices(); i ++) {
        std::cout << "v[" << i << "]: " << graph.vertex(i).data() << endl;
    }

    delete engine;
    return 0;
}
