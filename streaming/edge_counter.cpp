// Simple edge counting program
#include <iostream>

#include "streaming.hpp"

using namespace std;
using namespace sae::streaming;

struct InOutEdgeCounter {
    eid_t in_edges, out_edges;

    void init(const Context<InOutEdgeCounter>& context, const Vertex& v) {
        in_edges = out_edges = 0;
    }

    void edge(const Context<InOutEdgeCounter>& context, vid_t id, const Edge& e) {
        if (e.source == id) {
            out_edges ++;
        } else if (e.target == id) {
            in_edges ++;
        }
    }

    void output(const Context<InOutEdgeCounter>& context, vid_t id, std::ostream& os) const {
        os << id << " " << in_edges << " " << out_edges << "\n";
    }
};

int sgraph_main(StreamingGraph* g) {
    Context<InOutEdgeCounter> context;
    SinglePassRun(context, g);
    Output(context, cout);
    return 0;
}
