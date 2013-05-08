#ifndef SAE_ISERIALIZESTREAM_HPP
#define SAE_ISERIALIZESTREAM_HPP

#include <iostream>
#include <istream>
#include <type_traits>
#include <cstring>

namespace sae{
namespace serialization{

/*
 * Stream to serialize object
 */
class ISerializeStream {
    public:
        // serialize from memory
        const char* base;
        size_t off;
        size_t len;

        // serialize from istream
        std::istream* istr;

        ISerializeStream(const char* base, size_t len)
            :base(base), len(len), off(0), istr(nullptr) { }

        ISerializeStream(std::istream* istream)
            :base(nullptr), len(0), off(0), istr(istream) { }

        /*
         * Copy bytes from "this" to an address
         */
        void read(char* des, size_t l) {
            if (base) {
                memcpy(des, base + off, l);
                off += l;
            }else{
                istr->read(des, l);
            }
        }

        ~ISerializeStream() {}
};

/*
 * Semantic of custom serialization logic
 */
namespace custom_serialization_impl {

    template <typename serialize_stream_t, typename T>
    struct serialize_impl {
        inline static void run(serialize_stream_t& instr, T& t) {
            if (std::is_pod<T>::value) {
                instr.read(reinterpret_cast<char*>(&t), sizeof(T));
            } else {

            }
        }
    };
}

template <typename T>
inline ISerializeStream& operator>>(ISerializeStream& istream, T& t) {
    ::sae::serialization::custom_serialization_impl::serialize_impl< ISerializeStream, T >::run(istream, t);
    return istream;
}

}}
#endif
