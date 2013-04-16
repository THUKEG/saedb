#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <fstream>
#include "sae_include.hpp"

using namespace sae::io;
using namespace std;

struct VData {
    float weight;
};

struct EData {
    float weight;
};

typedef saedb::empty                    message_date_type;
typedef saedb::sae_graph<VData, EData>  graph_type;


void test_create() {
    sae::io::GraphBuilder<int, VData, EData> builder;

    for (int i=0; i<=11; ++i){
        builder.AddVertex(i, VData{0.1});
    }

    builder.AddEdge(0, 1, EData{0.1});
    builder.AddEdge(2, 1, EData{0.1});
    builder.AddEdge(3, 1, EData{0.4});
    builder.AddEdge(4, 0, EData{0.6});
    builder.AddEdge(5, 0, EData{0.1});
    builder.AddEdge(6, 2, EData{0.1});
    builder.AddEdge(7, 3, EData{0.2});
    builder.AddEdge(8, 3, EData{0.7});
    builder.AddEdge(9, 3, EData{0.1});
    builder.AddEdge(11, 4, EData{0.1});

    builder.Save("test_graph");
}


class Simulate:
public saedb::IAlgorithm<graph_type, float>
{
    /**
     * Simulate does not need gather.
     */
public:
    edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }

    gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        return 0.0;
    }

    void apply(icontext_type& context, vertex_type& vertex, const gather_type& total){
        // need not apply either
    }

    saedb::edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }

    void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {

        EData ed = edge.data();
        float weight = ed.weight;
        if ( ((double)rand() / RAND_MAX) < weight){
            context.signalVid(edge.source().id());
            std::cout << "vertex = "<< edge.source().id() << " is activated"<< std::endl;
        }
    }
};


struct ActiveNode {
    float num;

    ActiveNode(): num(0.0) {}
    ActiveNode(float f){
        this->num = f;
    }

    ActiveNode& operator+=(const ActiveNode& other){
        this->num += other.num;
        return *this;
    }
};

ActiveNode ActiveNodeAggregator(Simulate::icontext_type& context, const graph_type::vertex_type& vertex){
    return ActiveNode(1.0);
}


double *greedy_s;
bool *mark;

// give a seed u, to estimate how many nodes can be influenced by u
double simulate(saedb::IEngine<Simulate> *engine, const std::vector<uint32_t> &seedset, int round){
    // engine
    double result = 0.0;

    ActiveNode active_node_counter;

    for(int i=0;i<round;++i){
        engine->signalVertices(seedset);
        for(int j =0 ; j < seedset.size() ; j++){
            std::cout << "seed " << seedset[j] << std::endl;
        }
        engine->start();
        active_node_counter = engine->map_reduce_vertices<ActiveNode>(ActiveNodeAggregator);
        result += active_node_counter.num - seedset.size();
        std::cout << " result = " << result << std::endl;
    }

    // clean
    return result / (double)round;
}

int main(){

    test_create();

    std::string graph_path = "test_graph";
    graph_type g;
    g.load_format(graph_path);

    saedb::IEngine<Simulate> *engine = new saedb::EngineDelegate<Simulate>(g);

    size_t n = g.num_vertices();
    int round = 1;
    int ss_cnt = 2;
    //std::cin >> round >> ss_cnt;
    std::string outpath = "/Users/zhangjing0544/Dropbox/SAE/saedb/src/app/demo";

    // Initilization
    greedy_s = new double [ss_cnt+1];
    mark = new bool[n];

    std::priority_queue<std::pair<double, std::pair<int,int>>> q;
    std::vector<uint32_t> seed_list;
    // calculate the influenced number of nodes by each node, and store into q

    // calculate the influenced number of nodes by each node, and store into q
    for (int i = 0; i < n; i++) {
        std::pair<int, int> tp = std::make_pair(i, 1);
        seed_list.push_back(i);
        double val = simulate(engine, seed_list, round);
        std::cout << "veretex=" << i << ", marginal=" << val << std::endl;

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
                double val = simulate(engine, seed_list, round);
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
        std::cout << greedy_s[i] << std::endl;
    }
    result_file.close();

    delete greedy_s;
    delete mark;
    delete engine;
    return 0;
}
