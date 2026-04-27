#pragma once
#include <vector>
#include <tabledata.hpp>

enum RecordMethod { FREE_RECORD, INDEX_RECORD };

class ITable
{
public:
    virtual ~ITable() = default;

    virtual void loadTable(const std::string& filepath) = 0;

    virtual bool modify(const Record& record, int index) = 0;
    virtual bool add(const Record& record, int index) = 0;
    virtual bool remove(const std::string& key, int index) = 0;
    virtual TableResult find(const std::string& key, int index) = 0;
    
    virtual int indexOfRecord(const std::string& key, RecordMethod method) = 0;

    virtual std::vector<Record> getData() const = 0;
    virtual std::vector<std::string> getTitles() const = 0;

};