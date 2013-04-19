#ifndef SAE_TYPE_INFO_HPP
#define SAE_TYPE_INFO_HPP
#include <vector>
#include <memory>
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
};

/**
 * Access a field in a void* pointer
 */
struct FieldAccessor {

    FieldAccessor(void* base_ptr, const FieldInfo* field_info):
        fi(field_info)
    {
        base  = (char*)base_ptr;
    }

    template < typename field_t >
    field_t& getValue() {
        return *((field_t*)(base + fi->offset));
    }

    private:
    char* base;
    const FieldInfo* fi;
};

FieldInfo* CreateFieldInfo(const char*);
DataTypeInfo* CreateDataTypeInfo(const char*);

/*
 * now it can build and access type. Superman.
 */
struct DataTypeAccessor {

    // append field definition to current data type definition
    virtual void appendField(const char* fname, FieldType type) = 0;

    virtual void appendField(FieldInfo* fi) = 0;

    // get a vector of all field definitions
    virtual std::vector<FieldInfo*> getAllFields() = 0;

    virtual uint32_t getFieldCount() = 0;

    virtual const char* getTypeName() = 0;

    virtual size_t Size() = 0;

    virtual void print() = 0;

    // get a field by name
    // its actual job is just move forward void* and do type cast
    virtual FieldAccessor* getFieldAccessor(void* base, const char* name) = 0;

    /**
     * After building the type, clear allocated pointer
     * This should not be called when we are reading graph.
     */
    virtual void ClearAfterBuild() = 0;
};

/*
 * data type builder for user
 */
DataTypeAccessor* DataTypeAccessorFactory(const char*);

/*
 * data type build loaded from disk
 */
DataTypeAccessor* DataTypeAccessorFactory(DataTypeInfo*, FieldInfo*);

}}
#endif
