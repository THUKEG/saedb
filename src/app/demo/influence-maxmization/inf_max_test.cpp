#include <iostream>
#include <cmath>

#include "inf_max.hpp"
#include "test/testharness.hpp"

using namespace std;

class Empty {};

struct InfMaxTest {
	string filepath;

	InfMaxTest() {
		filepath = saedb::test::TempFileName();
		sae::io::GraphBuilder<int, VData, EData> builder;

		for (int i=0; i<11; ++i){
			builder.AddVertex(i, VData{0.1});
		}

		builder.AddEdge(0, 1, EData{0.3});
		builder.AddEdge(2, 1, EData{0.2});
		builder.AddEdge(3, 1, EData{0.8});
		builder.AddEdge(4, 0, EData{0.6});
		builder.AddEdge(5, 0, EData{0.3});
		builder.AddEdge(6, 2, EData{0.1});
		builder.AddEdge(7, 3, EData{0.2});
		builder.AddEdge(8, 3, EData{0.9});
		builder.AddEdge(9, 3, EData{0.1});
		builder.AddEdge(10, 4, EData{0.7});

		builder.Save(filepath.c_str());


	}

	~InfMaxTest() {
		// TODO remove temp graph files
	}
};

TEST(InfMaxTest, Inf_max) {

	srand(1);
	graph_type graph;
	graph.load_format(filepath);
	int ss_cnt = 11;
	int round = 10;
	vector<pair<int, double> > greedy_s = inf_max(graph,ss_cnt,round);
	printf("==== printf test result === \n");
	vector<int> seeds = {1, 0,4,9,7,6,2,8,10,3,5};
	vector<double> active_number = {2.9,5.0, 6.3, 7.7, 8.0, 8.8, 9.6, 9.9, 10.3, 10.4, 11};
	for (int ss = 0; ss < ss_cnt; ss++){
		printf("node id true= %d, calculated = %d, active number true = %lf, calculated = %lf\n", seeds[ss], greedy_s[ss].first, active_number[ss],greedy_s[ss].second);
		ASSERT_TRUE( (seeds[ss] - greedy_s[ss].first) == 0);
		ASSERT_TRUE( abs(active_number[ss] - greedy_s[ss].second ) <= 0.0001);
	}



	printf("==== test end =====\n");
}

int main(){
	saedb::test::RunAllTests();
}
