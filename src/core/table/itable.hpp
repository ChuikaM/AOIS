#pragma once
#include <iostream>
#include <filesystem>
#include <map>
#include <vector>

struct Record {
    std::vector<std::string> fields;
    bool isEmpty = false;
    bool isDeleted = false;
    bool viaCollision = false;
};

class ITable
{
public:
    virtual ~ITable() = default;

    virtual void Load(const std::string& filePath) = 0;
    virtual bool Modify(std::vector<std::string> fieldsNew) = 0;
    virtual bool Add(const Record& rec) = 0;
    virtual bool Delete(const std::string& key) = 0;
    virtual int Find(const std::string& key) = 0;

    virtual std::vector<Record> GetTableData() const = 0;

};