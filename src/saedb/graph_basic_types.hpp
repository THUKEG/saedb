#ifndef SAE_GRAPHBASIC_TYPE
#define SAE_GRAPHBASIC_TYPE

namespace saedb{
    typedef uint32_t vertex_id_type;

    typedef uint32_t lvid_type;

    typedef lvid_type edge_id_type;

    enum edge_dir_type {
        NO_EDGES,
        IN_EDGES,
        OUT_EDGES,
        ALL_EDGES
    };

    struct empty {
        void save() const { }
        void load() { }
        empty& operator+=(const empty&) { return *this; }
    };
}
#endif
