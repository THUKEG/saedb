#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <fstream>
#include <vector>
#include "sae_include.hpp"
#include "io/mgraph.hpp"
#include "io/graph_builder.hpp"
#include "Heap.h"

using namespace std;


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


vector<pair<int, double>> inf_max3(graph_type graph, double p, int ss_cnt){


	//    std::string graph_path = "test_graph";
	//    graph_type graph;
	//    graph.load_format(graph_path);


	std::cout << "#vertices: " << graph.num_vertices() << " #edges:" << graph.num_edges() << std::endl;


	vector<pair<int, double> > greedy_s;
	size_t n = graph.num_vertices();
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
	for (int ss = 0; ss < ss_cnt; ss++) {
		std::pair<double, int> max_marginal = heap->top();

		int seed_index = max_marginal.second;
		seed_list.push_back(seed_index);

		greedy_s.push_back(make_pair( seed_index, (graph.vertex(seed_index).num_in_edges() - t[seed_index])*p));
		//printf("current max marginal is %d, degree = %d, t=%d, value is %lf\n",
		//		max_marginal.second, graph.vertex(seed_index).num_in_edges(), t[seed_index], greedy_s[ss] );

		heap->pop();
		// modify its followees
		//std::cout << "number out edges="<< graph.vertex(seed_index).num_out_edges()<<std::endl;

		auto e = graph.vertex(seed_index).out_edges();

		for (int i = 0; i < graph.vertex(seed_index).num_out_edges(); i++) {

			e->MoveTo(i);
			auto v = e->TargetId();
			//std::cout << "current = " << v << std::endl;

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
			//std::cout << "modify index = " << v << ", dv = " << dv << ", tv = " << t[v] << ", p = " << p << ", marginal = " << dv - 2 * t[v] - (dv - t[v]) * t[v] * p;
			heap->modify(v, marginal);
			//            heap->print_heap();
		}

		//        printf("current selected seed = %d, marginal= %lf\n", seed_list[ss - 1],
		//                greedy_s[ss]);
	}

	for (int i = 0; i < ss_cnt; i++)
	{
		if(i!= 0){
			greedy_s[i].second += greedy_s[i-1].second;
		}
	}

	delete [] t;
	return greedy_s;

}

