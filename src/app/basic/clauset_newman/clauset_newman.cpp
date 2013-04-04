#include <iostream>
#include <cmath>

#include "sae_include.hpp"

using namespace saedb;

enum CNstatus
{
	INIT, FIND_MAX, UPDATE, DEAD
};

typedef struct edge_data_type_
{
	float deltaQ;
	edge_data_type_(float q = 0.0): deltaQ(q) {}
} edge_data_type;

typedef struct vertex_data_type_
{
	float a;
	bool alive;
	vertex_data_type_(float a_ = 0.0): a(a_)
	{
		alive = true;
	}
} vertex_data_type;

typedef empty message_data_type;
typedef sae_graph<vertex_data_type, edge_data_type> graph_type;

typedef struct gather_type_
{
	size_t i_id, j_id;
	float a_i, a_j;
	float max_del_q;
	bool cnt_i, cnt_j;
	float deltaQ_j_k;
	gather_type_(size_t i_id_, size_t j_id_, float a_i_, float a_j_, float max_del_q_):
		i_id(i_id_), j_id(j_id_), a_i(a_i_), a_j(a_j_), max_del_q(max_del_q_) 
	{
		cnt_i = false;
		cnt_j = false;
	}
	gather_type_()
	{
		cnt_i = false;
		cnt_j = false;
		max_del_q = -1e9;
	}
	void operator += (const gather_type_& other)
	{
		cnt_i |= other.cnt_i;
		cnt_j |= other.cnt_j;
		if (other.max_del_q > max_del_q)
		{
			a_i = other.a_i;
			a_j = other.a_j;
			i_id = other.i_id;
			j_id = other.j_id;
			max_del_q = other.max_del_q;
		}
		if (other.cnt_j) deltaQ_j_k = other.deltaQ_j_k;
	}
	gather_type_(bool bi, bool bj, float q = 0.0):
		cnt_i(bi), cnt_j(bj), deltaQ_j_k(q)
		{
		}
} gather_type;

class ClausetNewmanAggregator: public IAggregator
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

class QSumAggregator: public IAggregator
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

// Assume there is no more than one edge between every pair of vertices.
class ClausetNewman: public IAlgorithm <graph_type, gather_type>
{
private:
	gather_type max_cache;
	CNstatus status_;
	bool connected_i, connected_j;
	float q_j_k_cache;
public:
	void init(icontext_type& context, vertex_type& vertex)
	{
		status_ = INIT;
		size_t degree = vertex.num_in_edges() + vertex.num_out_edges();
		size_t num_edges = context.getNumEdges();
		vertex.data().a = float(degree) * 0.5 / float(num_edges);
		connected_i = false;
		connected_j = false;
	}

	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		if (status_ == INIT)
			return OUT_EDGES;
		else if (status_ == FIND_MAX)
		{
			IAggregator* agg = context.getAggregator("clauset_newman");
			gather_type gt;
			agg->init((void*)(&gt));
			return OUT_EDGES;
		}
		else if (status_ == UPDATE)
		{
			IAggregator* agg = context.getAggregator("clauset_newman");
			size_t i_id = ((gather_type*)(agg->data()))->i_id;
			size_t j_id = ((gather_type*)(agg->data()))->j_id;

			if (vertex.id() != i_id && vertex.id() != j_id)
				return ALL_EDGES;
			else
				return NO_EDGES;
		}
	}

	gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		if (status_ == INIT)
		{
			size_t edge_num = context.getNumEdges();
			edge.data().deltaQ = 0.5 / float (edge_num) - edge.source().data().a * edge.target().data().a;
		}
		else if (status_ == FIND_MAX)
		{
			if (edge.source().data().alive && edge.target().data().alive)
				return gather_type(edge.source().id(), edge.target().id(), edge.source().data().a, edge.target().data().a, edge.data().deltaQ);
			else 
				return gather_type();
		}
		else if (status_ == UPDATE)
		{
			IAggregator* agg = context.getAggregator("clauset_newman");
			size_t i_id = ((gather_type*)(agg->data()))->i_id;
			size_t j_id = ((gather_type*)(agg->data()))->j_id;
			if (edge.source().id() == i_id || edge.target().id() == i_id)
				return gather_type(true, false);
			else if (edge.source().id() == j_id || edge.target().id() == j_id)
				return gather_type(false, true, edge.data().deltaQ);
			else 
				return gather_type(false, false);
		}
	}

	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total)
	{
		if (status_ == FIND_MAX)
		{
			max_cache = total;
		}
		else if (status_ == UPDATE)
		{
			connected_i = total.cnt_i;
			connected_j = total.cnt_j;
			if (connected_j) q_j_k_cache = total.deltaQ_j_k;

			IAggregator* agg = context.getAggregator("clauset_newman");
			size_t j_id = ((gather_type*)(agg->data()))->j_id;
			if (j_id == vertex.id())
				vertex.data().alive = false;
		}
	}

	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
		if (status_ == UPDATE)
		{
			IAggregator* agg = context.getAggregator("clauset_newman");
			size_t i_id = ((gather_type*)(agg->data()))->i_id;
			size_t j_id = ((gather_type*)(agg->data()))->j_id;

			float del_q = ((gather_type*)(agg->data()))->max_del_q;
			if (del_q < 0) return NO_EDGES;

			if (i_id != vertex.id() && j_id != vertex.id())
			{
				return ALL_EDGES;
			}
		}

		return NO_EDGES;
	}

	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		IAggregator* agg = context.getAggregator("clauset_newman");
		size_t i_id = ((gather_type*)(agg->data()))->i_id;
		size_t j_id = ((gather_type*)(agg->data()))->j_id;
		float a_i = ((gather_type*)(agg->data()))->a_i;
		float a_j = ((gather_type*)(agg->data()))->a_j;

		if (edge.source().id() == i_id || edge.target().id() == i_id)
		{
			if (!connected_j)
				edge.data().deltaQ -= 2.0 * vertex.data().a * a_j;
			else
				edge.data().deltaQ += q_j_k_cache;
		}
		else if (edge.source().id() == j_id || edge.target().id() == j_id)
		{
			if (!connected_i)
				context.add_edge(vertex.id(), i_id, edge_data_type(edge.data().deltaQ - 2.0 * a_i * vertex.data().a));
		}
	}

	void aggregate(icontext_type& context, const vertex_type& vertex)
	{
		if (status_ == INIT)
			status_ = FIND_MAX;
		else if (status_ == FIND_MAX)
		{
			status_ = UPDATE;
			IAggregator* agg = context.getAggregator("clauset_newman");
			agg->reduce(&max_cache);
		}
		else if (status_ == UPDATE)
		{
			IAggregator* agg = context.getAggregator("clauset_newman");
			size_t j_id = ((gather_type*)(agg->data()))->j_id;
			size_t i_id = ((gather_type*)(agg->data()))->i_id;
			float del_q = ((gather_type*)(agg->data()))->max_del_q;

			if (vertex.id() == i_id)
			{
				IAggregator* sum_agg = context.getAggregator("QSUM");
				sum_agg->reduce((void*)(&del_q));
			}

			if (vertex.id() == j_id)
			{
				status_ = DEAD;
			}
			else if (del_q < 0)
				status_ = DEAD;
			else 	
				status_ = FIND_MAX;
		}

		if (status_ != DEAD) context.signal(vertex);
	}
};

int main()
{
}
