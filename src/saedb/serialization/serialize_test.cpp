#include "serialization_includes.hpp"
#include "../test/testharness.hpp"
#include <fstream>
#include <cstring>
#include <cstdio>

using namespace std;
using namespace sae::serialization;

struct SerializationTest {
    SerializationTest() {

    }

    ~SerializationTest() {
        // clean all bin file
        remove("int.bin");
        remove("char-star.bin");
        remove("string.bin");
        remove("char-array.bin");
    }
};

TEST(SerializationTest, SaveAndLoad) {
    int i = 42;
    int j;
    {
        std::ofstream fout("int.bin", std::fstream::binary);
        OSerializeStream encoder(&fout);
        encoder << i;
        fout.close();
    }

    {
        std::ifstream fin("int.bin", std::fstream::binary);
        ISerializeStream decoder(&fin);
        decoder >> j;
        ASSERT_EQ(i, j);
        fin.close();
    }
}

TEST(SerializationTest, BasicTypes) {
    const char* a = "hello world";
    const char* b;
    {
        std::ofstream fout("char-star.bin", std::fstream::binary);
        OSerializeStream encoder(&fout);
        encoder << a;
        fout.close();
    }

    {
        std::ifstream fin("char-star.bin", std::fstream::binary);
        ISerializeStream decoder(&fin);
        decoder >> b;
        ASSERT_TRUE(memcmp(a, b, sizeof(a)) == 0);
    }

    string s("source");
    string t;

    {
        std::ofstream fout("string.bin", std::fstream::binary);
        OSerializeStream encoder(&fout);
        encoder << s;
    }

    {
        std::ifstream fin("string.bin", std::fstream::binary);
        ISerializeStream decoder(&fin);
        decoder >> t;
        ASSERT_TRUE(memcmp(s.c_str(), t.c_str(), sizeof(s.c_str())) == 0);
    }

    char c[] = "hello world";
    char d[0];
    {
        std::ofstream fout("char-array.bin", std::fstream::binary);
        OSerializeStream encoder(&fout);
        encoder << c;
        fout.close();
    }

    {
        std::ifstream fin("char-array.bin", std::fstream::binary);
        ISerializeStream decoder(&fin);
        decoder >> d;
        ASSERT_TRUE(memcmp(c, d, sizeof(c)) == 0);
    }
}

int main(){
    return ::saedb::test::RunAllTests();
}
