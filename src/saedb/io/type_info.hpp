#ifndef SAE_TYPE_INFO_HPP
#define SAE_TYPE_INFO_HPP
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>

namespace sae{
namespace io{

#define MAX_TYPE_NAME_LEN 64
#define MAX_FIELD_NAME_LEN 256

// current support type
enum FieldType {
    INT_T = 0,
    FLOAT_T,
    DOUBLE_T
};

struct FieldInfo {
    char      field_name[MAX_FIELD_NAME_LEN];
    uint32_t  offset;
    uint32_t  size;
    FieldType type;
};

struct DataTypeInfo {
    char                         type_name[MAX_TYPE_NAME_LEN];
    uint32_t                     field_num;
    std::vector<FieldInfo>       fields;
};

FieldInfo* CreateFieldInfo(const char*);
DataTypeInfo* CreateDataTypeInfo(const char*);

// now it can build and access type. Superman.
struct DataTypeAccessor {

    DataTypeAccessor(const char* n) {
        strcpy(name, n);
        dt = CreateDataTypeInfo(n);
        dt->field_num = 0;
        position = 0;
    }
    // append field definition to current data type definition
    void appendField(const char* fname, FieldType type) {
        FieldInfo fi = FieldInfo();
        strcpy(fi.field_name, fname);
        fi.offset = position;
        fi.type = type;

        switch (type) {
            case INT_T:
                fi.size = sizeof(int32_t);
                position += sizeof(int32_t);
                break;
            case FLOAT_T:
                fi.size = sizeof(float);
                position += sizeof(float);
                break;
            case DOUBLE_T:
                fi.size = sizeof(double);
                position += sizeof(double);
                break;
        }
        dt->fields.push_back(std::move(fi));
        dt->field_num += 1;
    }

    // get a vector of all field definitions
    std::vector<FieldInfo> getAllFields() {
        return dt->fields;
    }


    uint32_t getFieldNum() {
        return dt->field_num;
    }

    void print() {
        std::cout << "struct " << name << std::endl;
        for (auto ft : dt->fields) {
            std::cout << "\tname:" << ft.field_name << ", " << "offset: " << ft.offset << ", size: " << ft.size << std::endl;
        }
    }
    // get a field by name
    // its actual job is just move forward void* and do type cast
    template < typename field_t >
    field_t getField(void* base, const char* name) {
        // quick impl
        for (auto ft : dt->fields) {
            if (strcmp(ft.field_name, name) == 0) {
                std::cout << "match name: " << ft.field_name << " and " << name << ", offset: " << ft.offset << std::endl;
                return *((field_t*)((char*)base + ft.offset));
            }
        }
        std::cout << "can not find field" << std::endl;
        return field_t();
    }

    private:
    char name[MAX_TYPE_NAME_LEN];
    DataTypeInfo* dt;
    uint32_t position;
};

// Build a builder
DataTypeAccessor* DataTypeAccessorFactory(const char* name);

}}
#endif
