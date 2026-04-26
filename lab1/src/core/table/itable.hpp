#pragma once
#include <vector>
#include <tabledata.hpp>

class ITable
{
public:
    virtual ~ITable() = default;

    virtual void loadTable(const std::string& filepath) = 0;

    virtual bool modify(const Record& record, int index) = 0;
    virtual bool add(const Record& record, int index) = 0;
    virtual bool remove(const std::string& key, int index) = 0;
    virtual bool find(const std::string& key, int index) = 0;
    
    virtual int indexOfRecord(const std::string& key) = 0;

    virtual std::vector<Record> getData() const = 0;
    virtual std::vector<std::string> getTitles() const = 0;

};