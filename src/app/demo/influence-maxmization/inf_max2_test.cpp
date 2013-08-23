#include <iostream>
#include <cmath>

#include "inf_max2.hpp"
#include "test/testharness.hpp"

using namespace std;

class Empty {};

struct InfMaxTest {
	string filepath;

	InfMaxTest() {
		filepath = saedb::test::TempFileName();
		sae::io::GraphBuilder<int, VData, EData> builder;

		for (int i=0; i<11; ++i){
			builder.AddVertex(i, VData{0, 0, 0.1});
		}

		builder.AddEdge(0, 1, EData{0,0.1});
		builder.AddEdge(2, 1, EData{0,0.2});
		builder.AddEdge(3, 1, EData{0,0.8});
		builder.AddEdge(4, 0, EData{0,0.6});
		builder.AddEdge(5, 0, EData{0,0.3});
		builder.AddEdge(6, 2, EData{0,0.1});
		builder.AddEdge(7, 3, EData{0,0.2});
		builder.AddEdge(8, 3, EData{0,0.9});
		builder.AddEdge(9, 3, EData{0,0.1});
		builder.AddEdge(10, 4, EData{0,0.7});


		builder.Save(filepath.c_str());


	}

	~InfMaxTest() {
		// TODO remove temp graph files
	}
};

TEST(InfMaxTest, Inf_max) {
	srand(10);
	graph_type graph;
	graph.load_format(filepath);
	int ss_cnt = 11;
	int round = 10000;
	vector<pair<int, double> > greedy_s = inf_max2(graph, ss_cnt, round);
	printf("==== printf test result === \n");
	vector<int> seeds = {1,10,9,8,7,6,5,4, 3,2,0};
	vector<double> active_number = {1,2,3,4,5,6,7,8,9,10,11};
	for (int ss = 0; ss < ss_cnt; ss++){
		printf("node id true= %d, calculated = %d, active number true = %lf, calculated = %lf\n", seeds[ss], greedy_s[ss].first, active_number[ss],greedy_s[ss].second);
		ASSERT_TRUE( (seeds[ss] - greedy_s[ss].first) == 0 || (seeds[ss-1] - greedy_s[ss].first) == 0 ||(seeds[ss+1] - greedy_s[ss].first) == 0);
		ASSERT_TRUE( abs(active_number[ss] - greedy_s[ss].second ) <= 0.8);
	}


	printf("==== test end =====\n");
}

int main(){

	saedb::test::RunAllTests();
}
