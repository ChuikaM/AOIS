#pragma once
#include <itable.hpp>

class TableBase : public ITable
{
public:
    ~TableBase() override = default;

    void load(const std::string& filepath = "./src/files/file.csv") override;

    std::vector<Record> getTableData() const override;
    virtual std::vector<Record> getData() const = 0;
    Record getDataAt(int index) const;
    std::vector<std::string> getTitles() const;

private:
    std::vector<std::string> m_parseCSVLine(const std::string& line);
 
    std::vector<std::string> m_titles;
    std::vector<Record> m_table_data;
    
};