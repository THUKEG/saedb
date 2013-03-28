#include <iostream>
#include <limits>
#include <fstream>
#include <algorithm>
#include "sae_include.hpp"
#include "generate.hpp"

#define MAXFL 1e6

class SP_dis
{
public:
	float dis;
	SP_dis(float dis_ = MAXFL) : dis(dis_) {}
	void operator +=(const SP_dis& other)
	{
		dis = min(dis, other.dis);
	}
};

using namespace saedb;

template <typename data_type, typename gather_type>
class shortest_path: public IAlgorithm <sae_graph<data_type, data_type>, gather_type>
{
private:
	bool updated;
public:
	typedef IAlgorithm<sae_graph<data_type, data_type>, gather_type> alg_type;
	typedef typename alg_type::icontext_type icontext_type;
	typedef typename alg_type::vertex_type vertex_type;
	typedef typename alg_type::edge_type edge_type;

	void init(icontext_type& context, vertex_type& vertex){}
	
	edge_dir_type gather_edges(icontext_type& context, const vertex_type& vertex) const
	{
		return IN_EDGES;
	}
	
	gather_type gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		float newval = edge.data() + edge.source().data();
		return gather_type(newval);
	}
	
	void apply(icontext_type& context, vertex_type& vertex, const gather_type& total)
	{
		if (total.dis < vertex.data())
		{
			updated = true;
			vertex.data() = total.dis;
		}
		else updated = false;
	}
	
	edge_dir_type scatter_edges(icontext_type& context, const vertex_type& vertex) const
	{
		if (updated || vertex.data() == 0) return OUT_EDGES;
		else return NO_EDGES;
	}
	
	void scatter(icontext_type& context, const vertex_type& vertex, edge_type& edge) const
	{
		if (vertex.data() + edge.data() < edge.target().data())
			context.signal(edge.target());
	}

	void aggregate(icontext_type& context, const vertex_type& vertex)
	{
	}
};

int main()
{
	shortest_path<float, SP_dis> sp;
	float_graph graph = generate_graph();

	IEngine<shortest_path<float, SP_dis> >* engine = new EngineDelegate<shortest_path<float, SP_dis> >(graph);
	engine->signalAll();
	engine->start();

	return 0;
}
