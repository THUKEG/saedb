#include "serialization_includes.hpp"
#include "../test/testharness.hpp"
#include <fstream>
#include <sstream>
#include <ostream>
#include <cstring>
#include <cstdio>
#include <list>
#include <vector>

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
        remove("list.bin");
        remove("vector.bin");
        remove("data.bin");
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
        ASSERT_TRUE(memcmp(a, b, strlen(a)) == 0);
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
        ASSERT_TRUE(memcmp(s.c_str(), t.c_str(), s.size()) == 0);
    }

    char c[] = "hello world";
    char d[12];
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
        //ASSERT_TRUE(memcmp(c, d, sizeof(c)) == 0);
    }
}

TEST(SerializationTest, List) {
    list<int> s;
    list<int> t;
    s.push_back(1);
    s.push_back(2);

    {
        ofstream fout("list.bin", std::fstream::binary);
        OSerializeStream encoder(&fout);
        encoder << s;
        fout.close();
    }

    {
        ifstream fin("list.bin", fstream::binary);
        ISerializeStream decoder(&fin);
        decoder >> t;
        ASSERT_EQ(t.size(), s.size());
        auto sbegin = s.begin();
        auto tbegin = t.begin();
        for (size_t i = 0;i < s.size(); ++i) {
            ASSERT_EQ(*sbegin, *tbegin);
            sbegin++;
            tbegin++;
        }
    }
}

TEST(SerializationTest, Vector) {
    vector<int> s;
    vector<int> t;
    s.push_back(1);
    s.push_back(2);

    {
        ofstream fout("vector.bin", fstream::binary);
        OSerializeStream encoder(&fout);
        encoder << s;
        fout.close();
    }

    {
        ifstream fin("vector.bin", fstream::binary);
        ISerializeStream decoder(&fin);
        decoder >> t;
        ASSERT_EQ(t.size(), s.size());
        auto sbegin = s.begin();
        auto tbegin = t.begin();
        for (size_t i = 0;i < s.size(); ++i) {
            ASSERT_EQ(*sbegin, *tbegin);
            sbegin++;
            tbegin++;
        }
    }
}


class Data {
    public:
    double i;
    vector<int> j;

    Data() {
    }
};

namespace sae{
namespace serialization{

namespace custom_serialization_impl{

    template <>
    struct serialize_impl<OSerializeStream, Data> {
        static void run(OSerializeStream& ostr, Data& d) {
            ostr << d.i << d.j;
        }
    };

    template <>
    struct deserialize_impl<ISerializeStream, Data> {
        static void run(ISerializeStream& istr, Data& d) {
            istr >> d.i >> d.j;
        }
    };
}}}

TEST(SerializationTest, CustomData) {
    Data a;
    a.i = 42;
    a.j.push_back(1);
    a.j.push_back(2);
    Data b;
    {
        ofstream fout("data.bin", fstream::binary);
        OSerializeStream encoder(&fout);
        encoder << a;
    }

    {
        ifstream fin("data.bin", fstream::binary);
        ISerializeStream decoder(&fin);
        decoder >> b;
        ASSERT_EQ(a.i, b.i);
        ASSERT_EQ(a.j.size(), b.j.size());
        auto abegin = a.j.begin();
        auto bbegin = b.j.begin();
        for (size_t i = 0;i < a.j.size(); ++i){
            ASSERT_EQ(*(abegin), *(bbegin));
            abegin++;
            bbegin++;
        }
    }
}

TEST(SerializationTest, StringStream) {
    Data a;
    a.i = 3.7;
    a.j.push_back(1);
    a.j.push_back(2);
    std::string tmp = convert_to_string(a);
    Data b = convert_from_string<Data>(convert_to_string(a));
    ASSERT_EQ(a.i, b.i);
    for (int t=0; t<a.j.size(); t++) {
        ASSERT_EQ(a.j[t], b.j[t]);
    }
}

struct data_type {
    int count;
    std::string name;
    data_type(int count = 0, std::string name = std::string("")) :
        count(count), name(name) {}
};
/*
TEST(SerializationTest, DataType) {
    std::vector<data_type> a;
    a.push_back(data_type(10, std::string("Kimiyoung")));
    ofstream fout("data_type.bin", fstream::binary);
    OSerializeStream encoder(&fout);
    encoder << a;
    fout.close();

    std::vector<data_type> b;
    ifstream fin("data_type.bin", fstream::binary);
    ISerializeStream decoder(&fin);
    decoder >> b;
    fin.close();

    ASSERT_EQ(a.size(), b.size());
    ASSERT_EQ(a[0].count, b[0].count);
    ASSERT_TRUE(a[0].name == b[0].name);
}
*/

int main(){
    return ::saedb::test::RunAllTests();
}
