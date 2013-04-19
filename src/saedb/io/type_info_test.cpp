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
    auto i_val = builder->getFieldAccessor(p, "i");
    auto j_val = builder->getFieldAccessor(p, "j");
    auto k_val = builder->getFieldAccessor(p, "k");

    if (!i_val || !j_val || !k_val) {
        cout << "Bug here. Can not find a field." << endl;
        return;
    }
    cout << i_val->getValue<uint32_t>() << endl;
    cout << j_val->getValue<float>() << endl;
    cout << k_val->getValue<double>() << endl;

    builder->ClearAfterBuild();
    delete builder;

    cout << "after data " << "i: " << t->i << ", j: " << t->j << ", k: " << t->k << endl;
}

int main() {
    test();
}
