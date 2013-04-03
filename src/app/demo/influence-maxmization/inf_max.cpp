#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <fstream>

#include "sae_include.hpp"
#include "sample_data.hpp"

typedef float                                               vertex_data_type;
typedef float                                               edge_data_type;
typedef saedb::empty                                        message_date_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type>  graph_type;

// count how many nodes are activated
class ActivateNodeAggregator: public saedb::IAggregator
{
public:
    void init(void* i){
        accu = *((float*)i);
    }

    void reduce(void* next){
        accu += 1.0;
    }

    void* data() const{
        return (void*)(&accu);
    }

    ~ActivateNodeAggregator() {}

private:
    float accu;
};

class Simulate:
public saedb::IAlgorithm<graph_type, float>
{
    /**
     * Simulate does not need gather.
     */
public:
    void init(icontext_type& context, vertex_type& vertex) {
    }

    edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }

    float gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        return 0.0;
    }

    void apply(icontext_type& context, vertex_type& vertex, const gather_type& total){
        // need not apply either
    }

    edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        float weight = edge.data();
        if ( ((double)rand() / RAND_MAX) < weight){
            context.signalVid(edge.target().id());
        }
    }

    void aggregate(icontext_type& context, const vertex_type& vertex){
        if (!marked) {
            marked = true;
            saedb::IAggregator* active_node = context.getAggregator("active_node");
            float t = vertex.data();
            active_node->reduce(&t);
        }
    }

private:
    bool marked = false;
};

graph_type sample_graph(){
    return LOAD_SAMPLE_GRAPH<graph_type>();
}

double *greedy_s;
bool *mark;

// give a seed u, to estimate how many nodes can be influenced by u
double simulate(saedb::IEngine<Simulate> *engine, graph_type& graph, const std::vector<uint32_t> &seedset, int round){
    // engine
    double result = 0.0;

    // aggregator
    float* init_rank = new float(0);
    saedb::IAggregator* active_node = new ActivateNodeAggregator();

    engine->registerAggregator("active_node", active_node);

    for(int i=0;i<round;++i){
        active_node->init(init_rank);
        engine->signalVertices(seedset);
        engine->start();
        result += *((float*)active_node->data()) - seedset.size();
    }

    // clean
    delete init_rank;
    delete active_node;

    return result / (double)round;
}

int main(){
    graph_type graph = sample_graph();
    // graph.load_format(graph_dir, format);

    std::cout << "#vertices: " << graph.num_vertices() << " #edges:" << graph.num_edges() << std::endl;

    saedb::IEngine<Simulate> *engine = new saedb::EngineDelegate<Simulate>(graph);

    size_t n = graph.num_vertices();
    int round = 1;
    int ss_cnt = 2;
    std::string outpath = "result";

    // Initilization
    greedy_s = new double [ss_cnt+1];
    mark = new bool[n];

    std::priority_queue<std::pair<double, std::pair<int,int>>> q;
    std::vector<uint32_t> seed_list;
    // calculate the influenced number of nodes by each node, and store into q
    for (int i = 0; i < n; i++) {
        std::pair<int, int> tp = std::make_pair(i, 1);
        seed_list.push_back(i);
        std::cout << "simulating ..." << i << std::endl;
        double val = simulate(engine, graph, seed_list, round);
        q.push(make_pair(val, tp));
        seed_list.clear();
    }

    std::cout << "Simulating all nodes done..." << std::endl;
    double ret = 0;
    // try ss_cnt seeds
    seed_list.clear();
    for (int ss = 1; ss <= ss_cnt; ss++) {
        while (1) {
            // fetch the node that can influence the most nodes
            std::pair<double, std::pair<int, int> > tp = q.top();
            q.pop();
            if (tp.second.second == ss) {
                ret += tp.first;
                greedy_s[ss] = ret;
                seed_list.push_back(tp.second.first);
                break;
            } else {
                seed_list.push_back(tp.second.first);
                double val = simulate(engine, graph, seed_list, round);
                seed_list.pop_back();
                tp.second.second = ss;
                tp.first = val - ret;
                q.push(tp);
            }
        }
    }

    std::ofstream result_file;
    result_file.open (outpath);
    for (int i = 1; i <= ss_cnt; i++)
    {
        result_file << greedy_s[i] << std::endl;
    }
    result_file.close();
    return 0;

    delete greedy_s;
    delete mark;
    delete engine;
    return 0;
}
