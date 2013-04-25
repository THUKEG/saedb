#include <cstdlib>
#include <iostream>
#include <string>
#include "test/testharness.hpp"
#include "mmap_file.hpp"

using namespace std;

struct MMapFileTest {
    MMapFileTest() {
        filepath = saedb::test::TempFileName();
    }

    ~MMapFileTest() {
        int ret = remove(filepath.c_str());
        ASSERT_TRUE(ret == 0) << "removing temp file: " << filepath;
    }

protected:
    string filepath;
};


TEST(MMapFileTest, CreateAndRead) {
    int COUNT = 17;
    // Test Create
    {
        int count = COUNT;
        int size = count * sizeof(int);
        MMapFile* f = MMapFile::Create(filepath.c_str(), size);
        ASSERT_TRUE(f) << "creating mmap file";
        int* d = (int*) f->Data();
        for (int i = 0; i < count; i++) {
            d[i] = i;
        }
        f->Close();
        delete f;
    }

    // Test Read
    {
        MMapFile* f = MMapFile::Open(filepath.c_str());
        ASSERT_TRUE(f) << "reading mmap file";

        int* d = (int*) f->Data();
        int count = f->Size() / sizeof(int);
        ASSERT_EQ(count, COUNT);
        for (int i = 0; i < count; i++) {
            ASSERT_EQ(d[i], i);
        }
        f->Close();
        delete f;
    }
}

int main() {
    return ::saedb::test::RunAllTests();
}
