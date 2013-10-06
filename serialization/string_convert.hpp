#pragma once

#include <sstream>
#include "iserializestream.hpp"
#include "oserializestream.hpp"

namespace sae {
    namespace serialization {
        template <typename T>
        T convert_from_string(std::string s) {
            T ret;
            std::stringstream stream;
            stream.str(s);
            ISerializeStream decoder(&stream);
            decoder >> ret;

            return ret;
        }

        template <typename T>
        std::string convert_to_string(const T& t) {
            std::stringstream stream;
            OSerializeStream encoder(&stream);
            encoder << t;

            return stream.str();
        }
    }
}
