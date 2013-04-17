#include <vector>
#include <cstring>
#include <iostream>
#include "type_info.hpp"

namespace sae{
namespace io{

FieldInfo* CreateFieldInfo(const char* name) {
    FieldInfo* ft = new FieldInfo;
    strcpy(ft->field_name, name);
    return ft;
}

DataTypeInfo* CreateDataTypeInfo(const char* name) {
    DataTypeInfo* dt = new DataTypeInfo;
    strcpy(dt->type_name, name);
    return dt;
}

DataTypeAccessor* DataTypeAccessorFactory(const char* name) {
    return new DataTypeAccessor(name);
}

}}
