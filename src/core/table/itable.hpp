#pragma once
#include <iostream>
#include <filesystem>
#include <map>

struct Record {
    std::string key;
    std::string field2;
    std::string field3;
    bool isEmpty = false;
    bool isDeleted = false;
    bool viaCollision = false;
};

class ITable
{
public:
    virtual ~ITable() = default;

    virtual void Load(const std::string& filePath) = 0;
    virtual bool Modify(const std::string& key, const std::string& newF2, const std::string& newF3) = 0;
    virtual bool Add(const Record& rec) = 0;
    virtual bool Delete(const std::string& key) = 0;
    virtual int Find(const std::string& key) = 0;

    virtual std::vector<Record> GetTableData() const = 0;

};