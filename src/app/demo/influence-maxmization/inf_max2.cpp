#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <fstream>
#include "sae_include.hpp"

using namespace std;


struct VData {
    bool active;
    bool signaled;
    float weight;
};

struct EData {
    bool cutted;
    float weight;
};

typedef saedb::empty                                        message_date_type;
typedef saedb::sae_graph<VData, EData>  graph_type;


void test_create() {
    sae::io::GraphBuilder<int, VData, EData> builder;
    
    for (int i=0; i<=11; ++i){
        builder.AddVertex(i, VData{false, false, 0.1});
    }
    
    builder.AddEdge(0, 1, EData{false, 0.1});
    builder.AddEdge(2, 1, EData{false, 0.1});
    builder.AddEdge(3, 1, EData{false, 0.4});
    builder.AddEdge(4, 0, EData{false, 0.6});
    builder.AddEdge(5, 0, EData{false, 0.1});
    builder.AddEdge(6, 2, EData{false, 0.1});
    builder.AddEdge(7, 3, EData{false, 0.2});
    builder.AddEdge(8, 3, EData{false, 0.7});
    builder.AddEdge(9, 3, EData{false, 0.1});
    builder.AddEdge(11, 4, EData{false, 0.1});
    
    
    
    builder.Save("test_graph");
}


typedef saedb::empty                                        message_date_type;
typedef saedb::sae_graph<VData, EData>  graph_type;

class UpdateGraph:
public saedb::IAlgorithm<graph_type, float>
{
public:
    edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }
    float gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        return 0.0;
    }
    void apply(icontext_type& context, vertex_type& vertex, const gather_type& total){
        vertex_data_type vertex_data = vertex.data();
        vertex_data.active = false;
    }
    edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }
    void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
    }
    void aggregate(icontext_type& context, const vertex_type& vertex){
    }
};

class RandomGraph:
public saedb::IAlgorithm<graph_type, float>
{
public:    
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
        edge_data_type edge_data = edge.data();
        float weight = edge_data.weight;        
        
        if ( ((double)rand() / RAND_MAX) <= (1 - weight)){            
            edge_data.cutted = true;
        }
        std::cout << "source id=" << edge.source().id() << ",target id=" << edge.target().id() << std::endl;
    }
};

class DFS:
public saedb::IAlgorithm<graph_type, float>
{
public:
    edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }
    float gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        return 0.0;
    }
    void apply(icontext_type& context, vertex_type& vertex, const gather_type& total){
        vertex_data_type vertex_data = vertex.data();
        vertex_data.active = true;
        std::cout << "vertex = "<< vertex.id() <<" is activated" << std::endl;
    }
    edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }
    void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        context.signalVid(edge.target().id());
    }
};

class DFS2:
public saedb::IAlgorithm<graph_type, float>
{
public:
    edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }
    float gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        return 0.0;
    }
    void apply(icontext_type& context, vertex_type& vertex, const gather_type& total){
        vertex.data().signaled = true;
    }
    edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }
    void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        vertex_data_type vertex_data = edge.target().data();
        if(!vertex_data.active){
            context.signal(edge.source());
            std::cout << "vertex = "<< edge.source().id() <<" is activated" << std::endl;

        }
    }
};

// to clear the "signaled" sign.
class DFS3:
public saedb::IAlgorithm<graph_type, float>
{
public:
    edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::NO_EDGES;
    }
    float gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        return 0.0;
    }
    void apply(icontext_type& context, vertex_type& vertex, const gather_type& total){
        vertex.data().signaled = false;
    }
    edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const{
        return saedb::IN_EDGES;
    }
    void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const {
        vertex_data_type vertex_data = edge.target().data();
        if(!vertex_data.active){
            context.signal(edge.source());
            std::cout << "vertex = "<< edge.source().id() <<" is cleared" << std::endl;

        }
    }
};

struct double_sum
{
    double value;
    double_sum(double value = 0.0): value(value) {}
    double_sum operator +=(const double_sum& other)
    {
        value += other.value;
        return *this;
    }
};

double_sum DoubleSumAggregator(DFS2::icontext_type& context, graph_type::vertex_type& vertex)
{
    if (vertex.data().signaled) return double_sum(1.0);
    return double_sum(0.0);
}


// randomly generate a graph from the original graph
void randomGraph(saedb::IEngine<RandomGraph> *engine){
    engine->signalAll();
    engine->start();
}

void dfs(saedb::IEngine<DFS> *engine, const std::vector<uint32_t> &seedset){
    engine->signalVertices(seedset);
    engine->start();
}

double dfs2(saedb::IEngine<DFS2> *engine, saedb::IEngine<DFS3> *clear_engine, const std::int32_t &seed){

    engine->signalVertex(seed);
    engine->start();
    // count the number of signaled vertices.
    double result = engine->map_reduce_vertices<double_sum>(DoubleSumAggregator).value - 1.0;

    clear_engine->signalVertex(seed);
    clear_engine->start();

    // clean
    return result;
}

void cleanGraph(saedb::IEngine<UpdateGraph> *engine){
    engine->signalAll();
    engine->start();
}




int main(){
    
    test_create();
    
    std::string graph_path = "test_graph";
    graph_type graph;
    graph.load_format(graph_path);
    
    // graph.load_format(graph_dir, format);
    
    std::cout << "#vertices: " << graph.num_vertices() << " #edges:" << graph.num_edges() << std::endl;
    saedb::IEngine<RandomGraph> *random_graph_engine = new saedb::EngineDelegate<RandomGraph>(graph);
    saedb::IEngine<DFS> *dfs_engine = new saedb::EngineDelegate<DFS>(graph);
    saedb::IEngine<DFS2> *dfs2_engine = new saedb::EngineDelegate<DFS2>(graph);
    saedb::IEngine<DFS3> *clear_engine = new saedb::EngineDelegate<DFS3>(graph);
    saedb::IEngine<UpdateGraph> *update_graph_engine = new saedb::EngineDelegate<UpdateGraph>(graph);
    
    
    size_t n = graph.num_vertices();
    double *greedy_s;
    
    int ss_cnt = 2;
    int R = 1;
    //std::cin >> R >> ss_cnt;
    std::string outpath = "/Users/zhangjing0544/Dropbox/SAE/saedb/src/app/demo";
    
    // Initilization
    
    greedy_s = new double [ss_cnt+1];
    double *marginal = new double[graph.num_vertices()];
    
    std::vector<uint32_t> seed_list;
    
      for (int ss = 1; ss <= ss_cnt; ss++) {
        for (int r = 0; r < R; r++) {
            //            printGraph();
            // random a graph
            randomGraph(random_graph_engine);
            //            printf("After shuffle");
            //printGraph();
            // get the collection of activated nodes by the seeds
            dfs(dfs_engine, seed_list);
            
            for (int i = 0; i < n; i++) {
                // if node is in the activated node collection, the marginal is set as 0
                //                printf("current node = %d\n", i);
                
                std::cout<< "vertex id:" << graph.vertex(i).id() <<std::endl;
                if (graph.vertex(i).data().active) {
                    continue;
                }
                marginal[i] = dfs2(dfs2_engine, clear_engine, i);
                std::cout<< "marginal value = " << marginal[i] <<std::endl;
               
            }
            
            cleanGraph(update_graph_engine);
        }
        double max_marginal = 0;
        int seed = -1;
        for (int i = 0; i < n; i++) {
            bool flag = false;
            for (int j = 0; j < seed_list.size(); j++) {
                if (seed_list[j] == i) {
                    flag = true;
                    break;
                }
            }
            if (flag)
                continue;
            marginal[i] /= R;
            if (max_marginal <= marginal[i]) {
                max_marginal = marginal[i];
                seed = i;
            }
            marginal[i] = 0;
        }
        greedy_s[ss] = max_marginal;
        seed_list.push_back(seed);
        std::cout<< "current selected seed = " << seed_list[ss-1] << "marginal= "<< greedy_s[ss] << std::endl;
    
    }
    
    
    
    for (int i = 1; i <= ss_cnt; i++)
    {
        std::cout << greedy_s[i] << std::endl;
    }
    
    delete greedy_s;    
    delete marginal;    
    delete random_graph_engine;
    delete dfs_engine;
    delete dfs2_engine;
    delete clear_engine;
    delete update_graph_engine;

    return 0;
}

