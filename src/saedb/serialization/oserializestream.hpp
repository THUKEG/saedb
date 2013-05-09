#ifndef SAE_OSERIALIZESTREAM_HPP
#define SAE_OSERIALIZESTREAM_HPP

#include <ostream>
#include <iostream>
#include <type_traits>

namespace sae{
namespace serialization{

/*
 * Steam to deserialize object
 */
class OSerializeStream {
    public:
        std::ostream* ostr;

        OSerializeStream(std::ostream* ostream)
            :ostr(ostream) { }

        /*
         * write bytes to stream
         */
        void write(const char* source, size_t len) {
            ostr->write(source, len);
        }

    ~OSerializeStream() {
        ostr->flush();
    }
};

/*
 * Semantic of custom serialization logic
 */
namespace custom_serialization_impl {

    template <typename stream_t, typename T>
    struct serialize_impl {
        static void run(stream_t& ostr, T& t) {
            if (std::is_pod<T>::value) {
                ostr.write(reinterpret_cast<char*>(&t), sizeof(T));
            } else {

            }
        }
    };

}

template <typename T>
OSerializeStream& operator<<(OSerializeStream& ostream, T& t) {
    custom_serialization_impl::serialize_impl< OSerializeStream, T >::run(ostream, t);
    return ostream;
}

}}
#endif
