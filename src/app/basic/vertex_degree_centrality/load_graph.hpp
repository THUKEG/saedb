#include <iostream>
#include <fstream>
#include "sae_include.hpp"

using namespace saedb;

typedef sae_graph<int, float> graph_type;

graph_type load_graph()
{
	graph_type graph;
	ifstream fin("input");
	int n;
	fin >> n;
	for (int i=0; i<n; i++) graph.add_vertex(i, 0);
	int m;
	fin >> m;
	for (int i=0; i<m; i++)
	{
		int x, y;
		fin >> x >> y;
		graph.add_edge(x, y, 0.0);
	}

	return graph;
}
