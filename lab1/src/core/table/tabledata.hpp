#pragma once
#include <vector>

struct Record {
    std::vector<std::string> fields;
    
    Record()
    {
        fields.reserve(3);
        fields.resize(3);
    }
    Record(std::vector<std::string> fields)
    {
        this->fields = fields;
    }

    bool operator!=(const Record& record) 
    {
        return fields != record.fields;
    }
    bool operator==(const Record& record) 
    {
        return fields == record.fields;
    }
};


struct TableContent {
    std::vector<Record> records;
    std::vector<std::string> titles;

};