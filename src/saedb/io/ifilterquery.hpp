#ifndef SAE_FILTER_QUERY_HPP
#define SAE_FILTER_QUERY_HPP

/*
 * Interface for filter query
 */
template < typename new_vdata_t, typename new_edata_t, typename old_vdata_t, typename old_edata_t >
class IFilterQuery {
    public:

        // predicate whether a vertex is selected
        virtual bool vertex_predicate(const old_vdata_t*) { return true; }

        // transform old vertex data to new vertex data
        virtual new_vdata_t vertex_transform(const old_vdata_t*) = 0;

        // transform old edge data to new edge data
        virtual new_edata_t edge_transform(const old_edata_t*) = 0;
};
#endif
