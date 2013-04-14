#include <iostream>
#include <limits>
#include <algorithm>
#include <memory>
#include "sae_include.hpp"
#include <cstdlib>
#include <cstring>

using namespace saedb;

#define EPS 1e-3
#define MAXFL (numeric_limits<double>::max())

// this is the gather type, which stores the minimum distance to be possibly updated for a vertex.
class SP_dis
{
public:
	double dis;
	SP_dis(const double&& dis = numeric_limits<double>::max()) : dis(dis) {}
	const SP_dis& operator +=(const SP_dis& other)
	{
		dis = min(dis, other.dis);
		return *this;
	}
};

// root_id is the id of the single source of shortest path.
int root_id; 

// graph mode determines whether the graph is directed or undirected.
enum GRAPH_MODE
{
	DIRECTED, UNDIRECTED
};
GRAPH_MODE graph_mode;

typedef saedb::sae_graph<double, double> double_graph;

template <typename data_type, typename gather_type>
class shortest_path: public IAlgorithm <sae_graph<data_type, data_type>, gather_type>
{
private:
	// Indicate whether the vertex data has been updated in a iteration.
	bool updated;
public:
	typedef IAlgorithm<sae_graph<data_type, data_type>, gather_type> alg_type;
	typedef typename alg_type::icontext_type icontext_type;
	typedef typename alg_type::vertex_type vertex_type;
	typedef typename alg_type::edge_type edge_type;
	typedef typename alg_type::message_type message_type;

	void init(icontext_type& context, vertex_type& vertex, const message_type& msg)
	{
		updated = false;
		if (vertex.id() == root_id)
			vertex.data() = 0.0;
		else
			vertex.data() = MAXFL;
	}
	
	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		if (graph_mode == DIRECTED)
			return IN_EDGES;
		else
			return ALL_EDGES;
	}
	
	gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		double newval;

		// data of the other vertex of the edge.
		double other_data = the_other_vertex_data(edge, vertex.id());
		
		if (other_data > MAXFL - EPS)
			newval = MAXFL;
		else // value to be gathered.
			newval = other_data + edge.data();

		return gather_type(std::move(newval));
	}
	
	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total)
	{
		// the vertex data could be updated.
		if (total.dis < vertex.data())
		{
			updated = true;
			vertex.data() = total.dis;
		}
		else updated = false;
	}
	
	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
		if (updated || vertex.id() == root_id) 
		{
			if (graph_mode == DIRECTED)
				return OUT_EDGES;
			else
				return ALL_EDGES;
		}
		else return NO_EDGES;
	}
	
	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		double other_data = the_other_vertex_data(edge, vertex.id());
		if (vertex.data() + edge.data() < other_data)
			context.signal(std::move(the_other_v(edge, vertex.id())));
	}

private:
	double the_other_vertex_data(const edge_type& edge, int vertex_id) const
	{
		if (edge.source().id() == vertex_id) return edge.target().data();
		return edge.source().data();
	}

	vertex_type the_other_v(const edge_type& edge, int vertex_id) const
	{
		if (edge.source().id() == vertex_id) return edge.target();
		return edge.source();
	}
};

// randomly create a graph, for debugging.
void random_create_graph()
{
	sae::io::GraphBuilder<int, double, double> builder;

	int n = 10000;
	for (int i=0; i<n; i++)
	{
		builder.AddVertex(i, 0.0);
	}
	int m = 100000;
	for (int i=0; i<m; i++)
	{
		int x = rand() % n, y = rand() % n;
		while (x == y)
			y = rand() % n;
		double z = rand() % 10000;
		builder.AddEdge(x, y, z);
	}

	builder.Save("shortest_path_graph");
}

int main(int argc, char*argv[])
{
//	random_create_graph();
	// by default, the root id is 0 and the graph is directed.
	root_id = 0;
	graph_mode = DIRECTED;
	std::string filepath = "shortest_path_graph";
	
	// to check the command line format.
	if (!(argc & 1))
	{
		std::cout << "Usage: ./shortest_path [-m DIRECTED/UNDIRECTED] [-r <root_id>] [-p <file path>]" << std::endl;
		return -1;
	}

	// to deal with command line options.
	for (int i=1; i<argc; i+=2)
	{
		if (strcmp(argv[i],"-m") == 0)
		{
			std::cout << argv[i+1] << std::endl;
			if (strcmp(argv[i+1], "UNDIRECTED") == 0) graph_mode = UNDIRECTED;
			else graph_mode = DIRECTED;
		}
		else if (strcmp(argv[i], "-r") == 0)
		{
			root_id = std::atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-p") == 0)
		{
			filepath = std::string(argv[i+1]);
		}
	}

	// load the graph.
	double_graph graph;
	try
	{
		graph.load_format(filepath);
		//TODO exception throwing.
	}
	catch (...)
	{
		std::cerr << "[Error] Loading format failed." << std::endl;
		std::cout << "Usage: ./shortest_path [-m DIRECTED/UNDIRECTED] [-r <root_id>] [-p <file path>]" << std::endl;
		return -1;
	}
	
	// run vertex programs.
	std::unique_ptr<IEngine<shortest_path<double, SP_dis> > > engine(new EngineDelegate<shortest_path<double, SP_dis> >(graph));
	engine->signalAll();
	engine->start();

	// output the shortest path for every vertex, to stdout.
	typedef double_graph::vertex_type vertex_type;
	int n = graph.num_vertices();
	for (int i=0; i<n; i++)
	{
		vertex_type v = graph.vertex(i);
		std::cout << "#" << v.id() << " :\t" << v.data() << std::endl;
	}


}
