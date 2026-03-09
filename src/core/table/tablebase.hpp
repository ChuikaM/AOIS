#pragma once
#include <itable.hpp>

class TableBase : private ITable
{
public:
    ~TableBase() override = default;

    void Load(const std::string& filepath = "./file/file.csv") override;

    std::vector<Record> GetTableData() const override;
    std::vector<std::string> GetTitles() const;

private:
    std::vector<std::string> parseCSVLine(const std::string& line);
 
    std::vector<std::string> m_titles;
    std::vector<Record> m_table_data;
    
};