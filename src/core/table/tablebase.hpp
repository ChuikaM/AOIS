#pragma once
#include <itable.hpp>

class TableBase : public ITable
{
public:
    ~TableBase() override = default;

    void Load(const std::string& filepath = "./files/file.csv") override;

    std::vector<Record> GetTableData() const override;
    virtual std::vector<Record> GetData() const = 0;
    Record GetDataAt(int index) const;
    std::vector<std::string> GetTitles() const;

private:
    std::vector<std::string> parseCSVLine(const std::string& line);
 
    std::vector<std::string> m_titles;
    std::vector<Record> m_table_data;
    
};