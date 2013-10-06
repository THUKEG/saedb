#ifndef SAE_MAPSERIALIZE_HPP
#define SAE_MAPSERIALIZE_HPP

#include <iterator>
#include <unordered_map>

namespace sae{
namespace serialization{

namespace custom_serialization_impl{
    template <typename T,typename V>
    struct serialize_impl<OSerializeStream, std::unordered_map<T,V>> {
        static void run(OSerializeStream& ostr, const std::unordered_map<T,V>& s){
            ostr << s.size();
            for (const auto& kv : s) {
                ostr << kv.first << kv.second;
            }
        }
    };

    template <typename T,typename V>
    struct deserialize_impl<ISerializeStream,std::unordered_map<T,V>> {
        static void run(ISerializeStream& istr, std::unordered_map<T,V>& s){
            size_t len;
            T k;
            V v;

            s.clear();
            istr >> len;

            for (size_t i = 0; i < len; i++) {
                istr >> k >> v;
                s.emplace(k, v);
            }
        }
    };

}}}

#endif

