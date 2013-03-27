#include <iostream>
#include "sae_include.hpp"
#include "load_graph.hpp"

class vertex_degree_centrality: public saedb::sae_algorithm<saedb::sae_graph<int, float>, int>
{
public:
	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return saedb::ALL_EDGES;
	}

	int gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		return 1;
	}

	void apply(icontext_type& context, vertex_type& vertex, const int& total)
	{
		vertex.data() = total;
	}

	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return saedb::NO_EDGES;
	}

	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
	}
};

int main()
{
	vertex_degree_centrality vdc;
	saedb::sae_graph<int, float> graph = load_graph();
	using namespace std;
	cout << "#vertices: " << graph.num_vertices() << " #edges: " << graph.num_edges() << endl;
	saedb::sae_synchronous_engine<vertex_degree_centrality> engine(graph);
	engine.signal_all();
	engine.start();
	return 0;
}
