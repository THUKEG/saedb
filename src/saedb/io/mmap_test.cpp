#include <iostream>
#include "mmap_file.hpp"

using namespace std;

void test_create() {
    int count = 17;
    int size = count * sizeof(int);
    MMapFile* f = MMapFile::Create("mapped", size);
    if (f) {
        int* d = (int*) f->Data();
        for (int i = 0; i < count; i++) {
            d[i] = i;
        }
        cout << "data: " << f->Data() << endl;
        cout << "size: " << f->Size() << endl;
        f->Close();
    } else {
        cout << "mmap failed" << endl;
    }
    delete f;
}

void test_read() {
    MMapFile* f = MMapFile::Open("mapped");
    if (f) {
        int* d = (int*) f->Data();
        int count = f->Size() / sizeof(int);
        for (int i = 0; i < count; i++) {
            cout << d[i] << endl;
        }
    } else {
        cout << "mmap failed" << endl;
    }
    f->Close();
    delete f;
}

int main() {
    test_create();
    test_read();
    return 0;
}
