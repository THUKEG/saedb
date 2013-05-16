#include <iostream>
#include <cmath>

#include "LazyForwardInfMax.hpp"
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
	int round = 1000;
	vector<pair<int, double> > greedy_s = inf_max(graph,ss_cnt,round);
	printf("==== printf test result === \n");
	//vector<int> seeds = {1, 0,6,9,2,7,5,4,3,10,8};
	vector<int> seed_position = {1,0,4,8,7,6,2,5,10,3,9};
	vector<double> active_number = {3.6,5.3,6.2,7.1,8.0,8.8,9.5,10.2,10.5,10.9,11};
	for (int i = 0 ; i < greedy_s.size() ; i++ ){
		printf("node = %d, true position= %d, calculated position= %d\n", greedy_s[i].first, seed_position[greedy_s[i].first], i);
		ASSERT_TRUE( abs(seed_position[greedy_s[i].first] - i) <=2 );
	}
	for (int ss = 0; ss < ss_cnt; ss++){
		printf("true active number = %lf, calculated active number = %lf\n",  active_number[ss],greedy_s[ss].second);
		ASSERT_TRUE( abs(active_number[ss] - greedy_s[ss].second ) <= 0.8);
	}



	printf("==== test end =====\n");
}

int main(){
	saedb::test::RunAllTests();
}
