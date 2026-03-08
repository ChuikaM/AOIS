#pragma once
#include <tablebase.hpp>
#include <map>

class TxtTable : public TableBase
{
public:
    explicit TxtTable(const std::string& filepath = "./file/file.csv");
    ~TxtTable() override = default;
    
    void Modify(const table::Cell& cell) override;
    void Add() override;
    void Delete() override;
    void Find() override;

private:
    std::map<std::string, std::vector<std::string>> m_table_data;
    
};