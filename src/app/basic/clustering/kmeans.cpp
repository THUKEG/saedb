#include <iostream>
#include <string>
#include "sae_include.hpp"

size_t NUM_CLUSTERS = 0;
bool IS_SPARSE = false;

typedef float vertex_data_type;
typedef float edge_data_type;
typedef saedb::empty message_data_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;



class cluster {
    cluster(): count(0), changed(false) {}
    std::vector<doble> center;

}

std::vector<cluster> CLUSTERS;

// the current cluster to initialize
size_t KMEANS_INITIALIZATION;

class vertex_data{


}

class edge_data{

}

// compute distance
double sqr_distance(const std::vector<double>& a, const std::vector<double>& b){
    ASSERT_EQ(a.size(), b.size());
}

// add two vectors
std::vector<double>& plus_equal_vector(std::vector<double>& a, const std::vector<double>& b){
    ASSERT_EQ(a.size(), b.size());
    for(size_t i=0; i<a.size(); ++i){
        a[i]+=b[i];
    }
    return a;
}

// scale a vector vetors
std::vector<double>& scale_vector(std::vector<double>& a, double d){

}


int main(int argc, char** argv){
    std::string datafile;
    std::string outcluster_file;
    std::string outdata_file;
    std::string edgedata_file;
    size_t MAX_ITERATION = 0;

    graphlab::mpi_tools::init(argc, argv);
    graphlab::distributed_control dc;
    // load graph
    graph_type graph();
    // don't understand what this is...
    NEXT_VID = (((graphlab::vertex_id_type)1<<31)/dc.numprocs()) * dc.procid();
    graph.load();
    graph.finalize();

    //start
    for(KMEANS_INITIALIZATION = 0; KMEANS_INITIALIZATION < NUM_CLUSTERS; KMEANS_INITIALIZATION++){
        if(IS_SPARSE == true){
            random_sample_reducer_sparse rs = graph.map_reduce_vertices<random_sample_reducer_sparse>(random_sample_reducer_sparse::get_weight);
            CLUSTERS[KMEANS_INITIALIZATION].center_sparse = rs.vtx;
            graph.transform_vertices(kmeans_pp_initialization_sparse);
        }else{
            random_sample_reducer_sparse rs = graph.map_reduce_vertices<random_sample_reducer_sparse>(random_sample_reducer_sparse::get_weight);
            CLUSTERS[KMEANS_INITIALIZATION].center = rs.vtx;
            graph.transform_vertices(kmeans_pp_initialization);
        }
    }

    graph_type graph = sample_graph();

    saedb::sae_synchronous_engine<pagerank> engine(graph);
    engine.start();
    return 0;
}
