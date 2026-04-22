#pragma once
#include <iostream>
#include <filesystem>
#include <map>
#include <vector>

struct Record {
    std::string key;
    std::vector<std::string> values;
    
    bool isEmpty = true;
    bool isDeleted = false;
};

class ITable
{
public:
    virtual ~ITable() = default;

    virtual void loadTable(const std::string& filepath) = 0;

    virtual bool modify(std::vector<std::string> fieldsNew, int index) = 0;
    virtual bool add(const Record& rec, int index) = 0;
    virtual bool remove(const std::string& key, int index) = 0;
    virtual bool find(const std::string& key, int index) = 0;

    virtual std::vector<Record> getData() const = 0;
    virtual std::vector<std::string> getTitles() const = 0;

};