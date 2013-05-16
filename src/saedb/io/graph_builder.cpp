#include "mgraph.hpp"
#include "graph_builder.hpp"

using namespace std;

namespace sae {
namespace io {

    GraphWriter* CreateMemoryMappedGraphWriter(const char * prefix, vid_t n, eid_t m, uint32_t vdata_size, uint32_t edata_size, uint32_t type_count, uint32_t type_total_size) {
        return MappedGraphWriter::Open(prefix, n, m, vdata_size, edata_size, type_count, type_total_size);
    }

}
}
