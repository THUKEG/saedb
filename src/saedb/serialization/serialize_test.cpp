#include "serialization_includes.hpp"
#include "../test/testharness.hpp"
#include <fstream>

using namespace std;
using namespace sae::serialization;

struct SerializationTest {
    SerializationTest() {

    }
};

TEST(SerializationTest, SaveAndLoad) {

    {
        float i = 42.43;
        std::ofstream fout("file.bin", std::fstream::binary);
        OSerializeStream encoder(&fout);
        encoder << i;
        cout << "Original: " << i << endl;
        fout.close();
    }

    {
        float j;
        std::ifstream fin("file.bin", std::fstream::binary);
        ISerializeStream decoder(&fin);
        decoder >> j;
        cout << "Current " << j << endl;
        fin.close();
    }
}

int main(){
    return ::saedb::test::RunAllTests();
}
