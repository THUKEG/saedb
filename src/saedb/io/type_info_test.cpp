#include "type_info.hpp"
#include <iostream>

using namespace std;
using namespace sae::io;

struct simple_type {
    int32_t i;
    float j;
    double k;
};

void test() {
    cout << "creating type builder" << endl;
    DataTypeAccessor* builder = DataTypeAccessorFactory("simple_type");

    cout << "appending type" << endl;
    builder->appendField("i", INT_T);
    builder->appendField("j", FLOAT_T);
    builder->appendField("k", DOUBLE_T);

    cout << endl << endl;
    builder->print();
    cout << endl << endl;

    simple_type* t = new simple_type;
    t->i = 100;
    t->j = 1.4;
    t->k = 2.5;

    cout << "original data " << "i: " << t->i << ", j: " << t->j << ", k: " << t->k << endl;
    void* p = (void*)t;
    cout << builder->getField<uint32_t>(p, "i") << endl;
    cout << builder->getField<float>(p, "j") << endl;
    cout << builder->getField<double>(p, "k") << endl;

    cout << "after data " << "i: " << t->i << ", j: " << t->j << ", k: " << t->k << endl;
}

int main() {
    test();
}
