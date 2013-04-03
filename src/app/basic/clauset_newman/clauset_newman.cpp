#include <iostream>
#include <cmath>

#include "sae_include.hpp"

using namespace saedb;

typedef struct edge_data_type_
{
	bool cnt_i, cnt_j;
	float deltaQ;
} edge_data_type;

typedef float vertex_data_type;
typedef empty message_data_type;
typedef sae_graph<vertex_data_type, edge_data_type> graph_type;

typedef struct gather_type_
{
	size_t i;
	size_t j;
	float deltaQ;
	gather_type_(size_t ii, size_t jj, float qq):
		i(ii), j(jj), deltaQ(qq) {}
	
	void operator +=(const gather_type_& other)
	{
		if (other.deltaQ > deltaQ)
			*this = other;
	}
} gather_type;

class DelQMaxAggregator: public IAggregator
{
public:
	void init(void* i)
	{
		accu = *((gather_type*) i);
	}
	void reduce(void* next)
	{
		accu += *((gather_type*) next);
	}
	void* data() const
	{
		return (void*)(&accu);
	}
private:
	gather_type accu;
};

enum clauset_newman_status
{
	FINDING_MAX, COUNTING_K, UPDATING, INITIATING
};

// Note that the algorithm is for undirected graph and all edges should be only stored once, say, if <i,j> exists then <j,i> should not exist.


class ClausetNewman: public IAlgorithm <graph_type, gather_type>
{
private:
	clauset_newman_status status_;
	gather_type max_cache_;
public:
	void init(icontext_type& context, vertex_type& vertex)
	{
		status_ = INITIATING;
		vertex.data() = float(vertex.num_in_edges() + vertex.num_out_edges()) / context.getNumEdges() / 2.0; // calculate a
	}

	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		if (status_ == INITIATING)
		{
			return OUT_EDGES;
		}
		else if (status_ == FINDING_MAX)
		{
			return OUT_EDGES;
		}
		else if (status_ == COUNTING_K)
		{
			IAggregator* del_q_max_aggregator = context.getAggregator("del_q_max_aggregator");
			gather_type* del_q_max_cache = (gather_type*)(del_q_max_aggregator->data());
			if (del_q_max_cache->i == vertex.id() || del_q_max_cache->j == vertex.id()) return ALL_EDGES;
		}
		else if (status_ == UPDATING)
		{
		}
	}

	gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		if (status_ == INITIATING)
		{
			float vertex_num = context.getNumVertices();
			edge.data().cnt_i = false;
			edge.data().cnt_j = false;
			edge.data().deltaQ = 0.5 / vertex_num - edge.source().data() * edge.target().data();
		}
		else if (status_ == FINDING_MAX)
		{
			return gather_type(edge.source().id(), edge.target().id(), edge.data().deltaQ);
		}
		else if (status_ == COUNTING_K)
		{
			IAggregator* del_q_max_aggregator = context.getAggregator("del_q_max_aggregator");
			gather_type* del_q_max_cache = (gather_type*)(del_q_max_aggregator->data());
			if (del_q_max_cache->i == vertex.id())
				edge.data().cnt_i = true;
			else if (del_q_max_cache->j == vertex.id())
				edge.data().cnt_j = true;
		}
	}

	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total)
	{
		if (status_ == FINDING_MAX)
		{
			max_cache_ = total;
		}
	}

	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
	}

	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
	}

	void aggregate(icontext_type& context, const vertex_type& vertex)
	{
		context.signal(vertex);
		if (status_ == INITIATING)
		{
			status_ = FINDING_MAX;
		}
		else if (status_ == FINDING_MAX)
		{
			IAggregator* del_q_max_agg = context.getAggregator("del_q_max_aggregator");
			del_q_max_agg->reduce(&max_cache_);
			status_ = COUNTING_K;
		}
		else if (status_ == COUNTING_K)
		{
			status_ = UPDATING;
		}
	}
};

int main()
{
}
