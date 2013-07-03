#ifndef SAE_ITERATOR_HPP
#define SAE_ITERATOR_HPP

#include <iterator>

namespace sae{
namespace serialization{

    template <typename iterator_t>
    void serialize_iterator(OSerializeStream& ostr, iterator_t begin, iterator_t end) {
        size_t len = std::distance(begin, end);
        ostr << len;

        for(;begin!=end;begin++) {
            ostr << (*begin);
        }
    }

    template <typename T, typename iterator_t>
    void deserialize_iterator(ISerializeStream& istr, iterator_t output) {
        size_t len;
        istr >> len;

        for(size_t i = 0; i < len; ++i) {
            T t;
            istr >> t;
            (*output) = t;
            output++;
        }
    }

}}
#endif
