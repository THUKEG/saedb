#include "mgraph.hpp"
#include "graph_builder.hpp"

using namespace std;

namespace sae {
namespace io {

    GraphWriter* CreateMemoryMappedGraphWriter(const char * prefix, vid_t n, eid_t m, uint32_t vdata_type_count, uint32_t edata_type_count, uint32_t * vertex_type_count, uint32_t* edge_type_count) {
        return MappedGraphWriter::Open(prefix, n, m, vdata_type_count, edata_type_count, vertex_type_count, edge_type_count);
    }

}
}
