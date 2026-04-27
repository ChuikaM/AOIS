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

    bool operator==(const Record& other) const
    {
        return fields == other.fields;
    }
    bool operator!=(const Record& other) const
    {
        return !(*this == other);
    }

};

struct TableContent {
    std::vector<Record> records;
    std::vector<std::string> titles;

};

struct TableResult {
    Record record;
    bool result;

};