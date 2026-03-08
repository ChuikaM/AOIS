#pragma once
#include <itable.hpp>

class TableBase : private ITable
{
public:
    ~TableBase() override = default;

    void Load(const std::string& filepath = "./file/file.csv") override;
    std::vector<std::vector<std::string>> GetTableData() const override
    {
        return m_table_data;
    }
    std::vector<std::string> GetTitles() const
    {
        return m_titles;
    }

private:
    std::vector<std::string> parseCSVLine(const std::string& line);
 
    std::vector<std::string> m_titles;
    std::vector<std::vector<std::string>> m_table_data;
    
};