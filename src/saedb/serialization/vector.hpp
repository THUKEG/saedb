#ifndef SAE_VECTORSERIALIZE_HPP
#define SAE_VECTORSERIALIZE_HPP

#include "iterator.hpp"
#include <iterator>
#include <vector>

namespace sae{
namespace serialization{

namespace custom_serialization_impl {
    template <typename T>
    struct serialize_impl<OSerializeStream, std::vector<T> > {
        static void run(OSerializeStream& ostr, std::vector<T>& s) {
            serialize_iterator(ostr, s.begin(), s.end());
        }
    };

    template <typename T>
    struct deserialize_impl<ISerializeStream, std::vector<T> > {
        static void run(ISerializeStream& istr, std::vector<T>& s) {
            s.clear();
            deserialize_iterator<T>(istr, std::inserter(s, s.end()));
        }
    };
};

}}
#endif
