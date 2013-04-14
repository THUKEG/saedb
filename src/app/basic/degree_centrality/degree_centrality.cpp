#include <iostream>
#include <cstring>

#include "sae_include.hpp"

using namespace saedb;

typedef int vertex_data_type;
typedef empty edge_data_type;
typedef sae_graph<vertex_data_type, edge_data_type> graph_type;

enum GRAPH_MODE
{
	UNDIRECTED, DIRECTED
};
GRAPH_MODE graph_mode;

class degree_centrality: public IAlgorithm<graph_type, empty>
{
public:
	void init(icontext_type& context, vertex_type& vertex, message_type& msg)
	{
		if (graph_mode == UNDIRECTED)
			vertex.data() = vertex.num_in_edges() + vertex.num_out_edges();
		else if (graph_mode == DIRECTED)
			vertex.data() = vertex.num_out_edges();
	}

	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return NO_EDGES;
	}

	empty gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		return empty();
	}

	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total)
	{
	}

	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return NO_EDGES;
	}

	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
	}
};

struct int_max
{
	int max_degree;
	int_max(int max_degree = 0): max_degree(max_degree) {}

	int_max operator += (const int_max& other)
	{
		max_degree = std::max(max_degree, other.max_degree);
		return *this;
	}
};

int_max intMaxAggregator(const graph_type::vertex_type& vertex)
{
	return int_max(vertex.data());
}

int main(int argc, char* argv[])
{
	graph_mode = UNDIRECTED;
	std::string filepath = "degree_centrality_graph";

	if (!(argc & 1))
	{
		std::cout << "Usage: ./degree_centrality [-m UNDIRECTED|DIRECTED] [-p <file path>]" << std::endl;
		return -1;
	}

	for (int i=1; i<argc; i+=2)
	{
		if (strcmp(argv[i], "-m") == 0)
		{
			if (strcmp(argv[i+1], "UNDIRECTED") == 0)
				graph_mode = UNDIRECTED;
			else if (strcmp(argv[i+1], "DIRECTED") == 0)
				graph_mode = DIRECTED;
		}
		else if (strcmp(argv[i], "-p") == 0)
		{
			filepath = std::string(argv[i+1]);
		}
	}

	graph_type graph;
	try
	{
		graph.load_format(filepath);
		//TODO throw exception
	}
	catch(...)
	{
		std::cerr << "[Error] Unknown Graph File Path" << std::endl;
		return -1;
	}

	IEngine<degree_centrality>* engine = new EngineDelegate<degree_centrality>(graph);
	engine->signalAll();
	engine->start();

	double max_degree = engine->map_reduce_vertices<int_max>(intMaxAggregator).max_degree;
	double sum_degree = graph.num_edges() * 2.0;
	double num_vertices = graph.num_vertices();

	double degree_centrality = (max_degree * num_vertices - sum_degree) / (num_vertices - 1.0) / (num_vertices - 2.0);
	std::cout << max_degree << ' ' << sum_degree << ' ' << num_vertices << std::endl;
	std::cout << "Degree Centrality of the graph: " << degree_centrality << std::endl;
}
