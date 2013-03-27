#include <iostream>
#include <fstream>
#include "sae_include.hpp"

using namespace saedb;

typedef sae_graph<float, float> float_graph;
typedef vertex_id_type id;

float_graph generate_graph()
{
	float_graph graph;

	ifstream fin("sp_input");
	int n;
	fin >> n;
	graph.add_vertex(0, 0);
	for (int i=1; i<n; i++)
		graph.add_vertex(i, 100);

	int m;
	fin >> m;
	for (int i=0; i<m; i++)
	{
		id x, y;
		float e;
		fin >> x >> y >> e;
		graph.add_edge(x, y, e);
	}

	return graph;
}
