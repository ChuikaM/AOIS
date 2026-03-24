#pragma once
#include <iostream>
#include <filesystem>
#include <map>
#include <vector>

struct Record {
    std::vector<std::string> fields;
    bool isEmpty = true;
    bool isDeleted = false;
};

class ITable
{
public:
    virtual ~ITable() = default;

    virtual bool modify(std::vector<std::string> fieldsNew) = 0;
    virtual bool add(const Record& rec) = 0;
    virtual bool remove(const std::string& key) = 0;
    virtual int find(const std::string& key) = 0;

    virtual std::vector<Record> getData() const = 0;
    virtual std::vector<std::string> getTitles() const = 0;

};