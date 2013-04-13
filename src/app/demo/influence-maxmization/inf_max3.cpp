#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <fstream>
#include "sae_include.hpp"
#include "io/mgraph.hpp"
#include "io/graph_builder.hpp"
#include "Heap.h"

struct VData {
    bool active;
    double weight;
};

struct EData {
    bool cutted;
    double weight;
};

typedef saedb::empty                                        message_date_type;
typedef saedb::sae_graph<VData, EData>  graph_type;


void test_create() {
    sae::io::GraphBuilder<int, VData, EData> builder;

    for (int i=0; i<=10; ++i){
        builder.AddVertex(i, VData{false, 0.1});
    }

    builder.AddEdge(0, 1, EData{false, 0.1});
    builder.AddEdge(2, 1, EData{false, 0.1});
    builder.AddEdge(3, 1, EData{false, 0.4});
    builder.AddEdge(4, 0, EData{false, 0.6});
    builder.AddEdge(4, 2, EData{false, 0.6});
    builder.AddEdge(5, 0, EData{false, 0.1});
    builder.AddEdge(6, 2, EData{false, 0.1});
    builder.AddEdge(4, 6, EData{false, 0.1});
    builder.AddEdge(7, 3, EData{false, 0.2});
    builder.AddEdge(8, 3, EData{false, 0.7});
    builder.AddEdge(9, 3, EData{false, 0.1});
    builder.AddEdge(10, 4, EData{false, 0.1});



    builder.Save("test_graph");
}

typedef saedb::empty                                        message_date_type;
typedef saedb::sae_graph<VData, EData>  graph_type;




int main(){

    test_create();

    std::string graph_path = "test_graph";
    graph_type graph;
    graph.load_format(graph_path);


    std::cout << "#vertices: " << graph.num_vertices() << " #edges:" << graph.num_edges() << std::endl;


    size_t n = graph.num_vertices();

    int ss_cnt = 10;
    double p = 0.1;

    double *greedy_s;
    greedy_s = new double[ss_cnt + 1];
	for (int i = 0 ; i < ss_cnt+1 ; i++){
		greedy_s[i] = 0;
	}

    int *t;
    t = new int[n];
	for (int i = 0; i < n; i++) {
		t[i] = 0;
	}

    std::vector<int> seed_list;

    std::Heap* heap = new std::Heap();

	for (int i = 0; i < n; i++) {
		heap->push(i, graph.vertex(i).num_in_edges());
	}
    heap->print_heap();
    for (int ss = 1; ss <= ss_cnt; ss++) {

		std::pair<double, int> max_marginal = heap->top();
		int seed_index = max_marginal.second;
		seed_list.push_back(seed_index);

		greedy_s[ss] =  greedy_s[ss-1]+ (graph.vertex(seed_index).num_in_edges() - t[seed_index])*p;
		printf("current max marginal is %d, degree = %d, t=%d, value is %lf\n",
               max_marginal.second, graph.vertex(seed_index).num_in_edges(), t[seed_index], greedy_s[ss] );

		heap->pop();
        // modify its followees
        std::cout << "number out edges="<< graph.vertex(seed_index).num_out_edges()<<std::endl;

        auto e = graph.vertex(seed_index).out_edges();

		for (int i = 0; i < graph.vertex(seed_index).num_out_edges(); i++) {

			e->MoveTo(i);
            auto v = e->TargetId();
			std::cout << "current = " << v << std::endl;

			bool flag = false;
			for (int j = 0; j < seed_list.size(); j++) {
				if (seed_list[j] == v) {
					flag = true;
					break;
				}
			}
			if (flag)
				continue;
			t[v]++;
			int dv = graph.vertex(i).num_in_edges();
			double marginal = dv - 2 * t[v] - (dv - t[v]) * t[v] * p;
 //           printf("modify index = %d, dv = %d, tv = %d, p= %lf, marginal= %lf \n", v, dv, t[v], p, dv - 2 * t[v] - (dv - t[v]) * t[v] * p);
			std::cout << "modify index = " << v << ", dv = " << dv << ", tv = " << t[v] << ", p = " << p
			<< ", marginal = " << dv - 2 * t[v] - (dv - t[v]) * t[v] * p << std::endl;
			heap->modify(v, marginal);
            //			heap->print_heap();
		}

        //		printf("current selected seed = %d, marginal= %lf\n", seed_list[ss - 1],
        //				greedy_s[ss]);
	}

    for (int i = 1; i <= ss_cnt; i++)
    {
        std::cout << greedy_s[i] << std::endl;
    }



    delete greedy_s;
    return 0;

}

