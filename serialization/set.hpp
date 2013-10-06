#ifndef SAE_SETSERILIZE_HPP
#define SAE_SETSERILIZE_HPP

#include "iterator.hpp"
#include <iterator>
#include <set>

namespace sae{
namespace serialization{

namespace custom_serialization_impl{
    template<typename T>
    struct serialize_impl<OSerializeStream, std::set<T>> {
        static void run(OSerializeStream& ostr, std::set<T>& s) {
            serialize_iterator(ostr, s.begin(), s.end());
        }
    };

    template <typename T>
    struct deserialize_impl<ISerializeStream, std::set<T>> {
        static void run(ISerializeStream& istr, std::set<T>& s) {
            s.clear();
            deserialize_iterator<T>(istr, std::inserter(s, s.end()));
        }
    };
}
}
}
#endif

