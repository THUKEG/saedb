#include <iostream>
#include <cmath>

#include "DegreeDiscountInfMax.hpp"
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

	    builder.AddEdge(0, 1, EData{false, 0.1});
	    builder.AddEdge(2, 1, EData{false, 0.1});
	    builder.AddEdge(3, 1, EData{false, 0.1});
	    builder.AddEdge(4, 0, EData{false, 0.1});
	    builder.AddEdge(5, 0, EData{false, 0.1});
	    builder.AddEdge(6, 2, EData{false, 0.1});
	    builder.AddEdge(7, 3, EData{false, 0.1});
	    builder.AddEdge(8, 3, EData{false, 0.1});
	    builder.AddEdge(9, 3, EData{false, 0.1});
	    builder.AddEdge(10,4, EData{false, 0.1});


		builder.Save(filepath.c_str());


	}

	~InfMaxTest() {
		// TODO remove temp graph files
	}
};

TEST(InfMaxTest, Inf_max) {

	graph_type graph;
	graph.load_format(filepath);
	double p = 0.1;
	int ss_cnt = 11;
	vector<pair<int, double> > greedy_s = inf_max3(graph,p, ss_cnt);
	printf("==== printf test result === \n");
	vector<int> seeds = {1,3,0,4,2,6,5,9,10,7,8};
	vector<double> active_number = {0.3,0.6,0.8,0.9,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
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
