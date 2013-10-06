#ifndef SAE_BASICSERIALIZE_HPP
#define SAE_BASICSERIALIZE_HPP

#include <cstring>
#include <iostream>

namespace sae{
namespace serialization{
    class ISerializeStream;
    class OSerializeStream;
}}

namespace sae{
namespace serialization{

namespace custom_serialization_impl {

    /*
     * Char*
     */
    template <>
    struct deserialize_impl<ISerializeStream, char*> {
        static void run(ISerializeStream& istr, char*& des) {
            size_t len;
            istr >> len;
            des = new char[len];
            istr.read(reinterpret_cast<char*>(des), len);
        }
    };

    template <>
    struct serialize_impl<OSerializeStream, char*> {
        static void run(OSerializeStream& ostr, const char * source) {
            size_t len = strlen(source);
            len++;
            ostr << len;
            ostr.write(source, len);
        }
    };

    /*
     * Char Array
     */
    template <size_t length>
    struct deserialize_impl<ISerializeStream, char[length]> {
        static void run(ISerializeStream& istr, char des[length]) {
            size_t len;
            istr >> len;
            // TODO assert len == length
            istr.read(reinterpret_cast<char*>(des), len);
        }
    };

    template <size_t length>
    struct serialize_impl<OSerializeStream, char[length]> {
        static void run(OSerializeStream& ostr, const char source[length]) {
            size_t len = length;
            ostr << len;
            ostr.write(reinterpret_cast<const char*>(source), len);
        }
    };


    /*
     * String
     */
    template <>
    struct deserialize_impl<ISerializeStream, std::string> {
        static void run(ISerializeStream& istr, std::string& des) {
            size_t len;
            istr >> len;
            des.resize(len);
            // Writing to string's buffer is undefined behavior.
            // It's guranteed to be continous, though.
            istr.read(const_cast<char*>(des.data()), len);
        }
    };

    template <>
    struct serialize_impl<OSerializeStream, std::string> {
        static void run(OSerializeStream& ostr, const std::string& source) {
            size_t len = source.length();
            ostr << len;
            ostr.write(source.c_str(), len);
        }
    };
}

}}
#endif
