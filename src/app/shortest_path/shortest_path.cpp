#include <iostream>
#include <limits>
#include <fstream>
#include <string>
#include "sae_include.hpp"
#include "sample_data.hpp"

#define INFILE "input"
#define MAX (100)

typedef float vertex_data_type;
typedef float edge_data_type;
typedef saedb::empty message_data_type;
typedef saedb::sae_graph<vertex_data_type, edge_data_type> graph_type;

class SP_dis
{
public:
	float dis;
	void operator +=(const SP_dis& other)
	{
		if (other.dis < dis) dis = other.dis;
	}
	SP_dis(float dis_ = MAX): dis(dis_) {}
};

class shortest_path: public saedb::sae_algorithm<graph_type, SP_dis>
{
public:
	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return saedb::IN_EDGES;
	}

	SP_dis gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		float newval = edge.source().data() + edge.data();
		return SP_dis(newval);
	}

	void apply(icontext_type& context, vertex_type& vertex, const SP_dis& total)
	{
		if (total.dis < vertex.data())
			vertex.data() = total.dis;
	}

	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return saedb::OUT_EDGES;
	}

	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		// TODO signal funciton is not yet implemented
		if (vertex.data() + edge.data() < edge.target().data())
			context.signal(edge.target());
	}
};

graph_type generate_graph()
{
	graph_type graph;
	using namespace std;
	ifstream fin(INFILE);

	int n;
	fin >> n;
	graph.add_vertex(0, 0);
	for (int i=1; i<n; i++)
		graph.add_vertex(i, MAX);

	int m;
	fin >> m;
	for (int i=0; i<m; i++)
	{
		saedb::vertex_id_type x, y;
		float e;
		fin >> x >> y >> e;
		graph.add_edge(x, y, e);
	}

	return graph;
}

int main()
{
	graph_type graph = generate_graph();

	using namespace std;
	cout << "#vertices: " << graph.num_vertices() << " #edges: " << graph.num_edges() << endl;

	saedb::sae_synchronous_engine<shortest_path> engine(graph);
	//engine.signal(vertex_type(&graph, 0));
	engine.start();
	cout << "Done." << endl;

	return 0;
}
