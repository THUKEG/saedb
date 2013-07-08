#include <iostream>
#include "io/mgraph.hpp"

using namespace std;
using namespace sae::io;

struct vertex_data_type {
	bool isSeed;
	bool inside;
	bool changed;
	int alpha;
	int beta;

	vertex_data_type() {
		alpha = 0;
		beta = 0;
		isSeed = false;
		inside = false;
		changed = true;
	}

	vertex_data_type(int i) {
		alpha = 0;
		beta = 0;
		isSeed = true;
		inside = 1;
		changed = true;
	}
};

typedef float edge_data_type;

void test_create() {
    sae::io::GraphBuilder<int> builder;

    builder.AddVertex(0, vertex_data_type(1));
    builder.AddVertex(10, vertex_data_type(1));
    builder.AddVertex(20, vertex_data_type());
    builder.AddVertex(30, vertex_data_type{});
	builder.AddVertex(40, vertex_data_type());
	builder.AddVertex(50, vertex_data_type{});

    builder.AddEdge(0, 10, float{10});
	builder.AddEdge(0, 20, float{10});
	builder.AddEdge(0, 30, float{10});
	builder.AddEdge(10, 40, float{10});

    builder.Save("alpha_beta_graph");
}


int main(int argc, const char * argv[]) {
    if (argc == 1 || (argc > 1 && argv[1][0] == 'c')) test_create();
}
