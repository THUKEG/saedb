#include <iostream>
#include <cmath>

#include "sae_include.hpp"
#include "sample_data.hpp"

using namespace saedb;

typedef float vertex_data_type;
typedef float edge_data_type;
typedef empty message_data_type;
typedef sae_graph<vertex_data_type, edge_data_type> graph_type;

class FloatMaxAggregator: public IAggregator
{
public:
	void init(void* i)
	{
		accu = *((float*) i);
	}
	void reduce(void* next)
	{
		accu = max(accu, *((float*) next));
	}
	void* data() const
	{
		return (void*)(&accu);
	}

private:
	float accu;
};

class FloatSumAggregator: public IAggregator
{
public:
	void init(void* i)
	{
		accu = *((float*) i);
	}
	void reduce(void* next)
	{
		accu += *((float*) next);
	}
	void* data() const
	{
		return (void*)(&accu);
	}
private:
	float accu;
};

class degree_centrality:
	public IAlgorithm <graph_type, float>
{
private:
	edge_dir_type MODE;
public:
	degree_centrality(edge_dir_type MODE_ = ALL_EDGES): MODE(MODE_) {}

	void init(icontext_type& context, vertex_type& vertex)
	{
		vertex.data() = 0.0;
	}

	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return MODE;
	}

	float gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		return 1.0;
	}

	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total)
	{
		vertex.data() = total;
	}

	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return NO_EDGES;
	}

	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
	}

	void aggregate(icontext_type& context, const vertex_type& vertex)
	{
		IAggregator* max_aggregator = context.getAggregator("max_degree_centrality");
		float tmp = vertex.data();
		max_aggregator->reduce(&tmp);

		IAggregator* sum_aggregator = context.getAggregator("sum_degree_centrality");
		float t = vertex.data();
		sum_aggregator->reduce(&t);
	}
};

int main()
{
	graph_type graph = LOAD_SAMPLE_GRAPH<graph_type>();
	IEngine<degree_centrality> *engine = new EngineDelegate<degree_centrality>(graph);

	IAggregator* max_degree_centrality = new FloatMaxAggregator();
	max_degree_centrality->init(new float(0.0));
	IAggregator* sum_degree_centrality = new FloatSumAggregator();
	sum_degree_centrality->init(new float(0.0));

	engine->registerAggregator("max_degree_centrality", max_degree_centrality);
	engine->registerAggregator("sum_degree_centrality", sum_degree_centrality);

	engine->signalAll();
	engine->start();

	/*
	Calculation of degree centrality of a given graph.
	REF: http://en.wikipedia.org/wiki/Centrality#cite_note-2
	*/

	float max_degree = *((float*)max_degree_centrality->data());
	float sum_degree = *((float*)sum_degree_centrality->data());
	float num_vertex = (float)graph.num_vertices();
	cout << "Degree Centrality of the graph: " << (max_degree * num_vertex - sum_degree) / (num_vertex - 1) / (num_vertex - 2) << endl;

	delete engine;
	return 0;	
}
