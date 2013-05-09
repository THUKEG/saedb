#ifndef SAE_LISTSERILIZE_HPP
#define SAE_LISTSERILIZE_HPP

#include "iterator.hpp"
#include <iterator>
#include <list>

namespace sae{
namespace serialization{

namespace custom_serialization_impl{
    template<typename T>
    struct serialize_impl<OSerializeStream, std::list<T>> {
        static void run(OSerializeStream& ostr, std::list<T>& s) {
            serializa_iterator(ostr, s.begin(), s.end());
        }
    };

    template <typename T>
    struct deserialize_impl<ISerializeStream, std::list<T>> {
        static void run(ISerializeStream& istr, std::list<T>& s) {
            s.clear();
            deserialize_iterator<T>(istr, std::inserter(s, s.end()));
        }
    };
}
}
}
#endif
