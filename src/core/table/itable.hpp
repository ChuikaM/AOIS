#pragma once
#include <iostream>
#include <filesystem>
#include <map>

namespace table
{
    struct Cell
    {
        int column;
        int row;
    };

    struct Record
    {
        std::map<std::string, std::string> data;
    };

}

class ITable
{
public:
    virtual ~ITable() = default;

    virtual void Load(const std::string& filePath) = 0;
    virtual void Modify(const table::Cell& cell) = 0;
    virtual void Add() = 0;
    virtual void Delete() = 0;
    virtual void Find() = 0;

    virtual std::vector<std::vector<std::string>> GetTableData() const = 0;

};