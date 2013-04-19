#include <vector>
#include <memory>
#include <cstring>
#include <iostream>
#include "type_info.hpp"

namespace sae{
namespace io{

// now it can build and access type. Superman.
struct DataTypeAccessorImpl: public DataTypeAccessor {

    /*
     * construct a data type bu user
     */
    DataTypeAccessorImpl(const char* n) {
        dt = CreateDataTypeInfo(n);
        dt->field_num = 0;
        position = 0;
    }

    /**
     * construct an accessor from memory mapped file
     */
    DataTypeAccessorImpl(DataTypeInfo* ti, FieldInfo* fi) {
        dt = ti;
        uint32_t field_count = dt->field_num;
        while (field_count > 0) {
            fields.push_back(fi);
            fi++;
            field_count--;
        }
    }

    // append field definition to current data type definition
    void appendField(const char* fname, FieldType type) {
        FieldInfo* fi = new FieldInfo;
        strcpy(fi->field_name, fname);
        fi->offset = position;
        fi->type = type;

        switch (type) {
            case INT_T:
                fi->size = sizeof(int32_t);
                position += sizeof(int32_t);
                break;
            case FLOAT_T:
                fi->size = sizeof(float);
                position += sizeof(float);
                break;
            case DOUBLE_T:
                fi->size = sizeof(double);
                position += sizeof(double);
                break;
        }
        fields.push_back(fi);
        dt->field_num += 1;
    }

    void appendField(FieldInfo* fi) {
        fields.push_back(fi);
    }

    // get a vector of all field definitions
    std::vector<FieldInfo*> getAllFields() {
        return fields;
    }

    uint32_t getFieldCount() {
        return dt->field_num;
    }

    const char* getTypeName() {
        return dt->type_name;
    }

    size_t Size() {
        return sizeof(DataTypeInfo) + fields.size() * sizeof(FieldInfo);
    }

    void print() {
        std::cout << "struct " << dt->type_name << std::endl;
        for (auto ft : fields) {
            std::cout << "\tname:" << ft->field_name << ", " << "offset: "
                      << ft->offset << ", size: " << ft->size << std::endl;
        }
    }
    // get a field by name
    // its actual job is just move forward void* and do type cast
    FieldAccessorPtr getFieldAccessor(void* base, const char* name) {
        // quick impl
        for (auto ft : fields) {
            if (strcmp(ft->field_name, name) == 0) {
                std::cout << "match name: " << ft->field_name << " and " << name << ", offset: " << ft->offset << std::endl;
                return std::unique_ptr<FieldAccessor>(new FieldAccessor(base, ft));
            }
        }
        return nullptr;
    }

    void ClearAfterBuild() {
        delete dt;
        for (auto p : fields) {
            delete p;
        }
    }


    private:
    DataTypeInfo* dt;
    uint32_t position;
    std::vector<FieldInfo*> fields;
};


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
    return new DataTypeAccessorImpl(name);
}

DataTypeAccessor* DataTypeAccessorFactory(DataTypeInfo* dti, FieldInfo* fi) {
    return new DataTypeAccessorImpl(dti, fi);
}


}}
