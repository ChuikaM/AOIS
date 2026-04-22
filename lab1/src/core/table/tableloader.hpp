#pragma once
#include <itable.hpp>

struct TableContent {
    std::vector<Record> records;
    std::vector<std::string> titles;

};

class TableLoader
{
public:
    explicit TableLoader();
    ~TableLoader() = default;

    static TableContent loadFromFile(const std::string& filepath);
    
private:
    std::vector<Record> m_load(const std::string& filepath);
    std::vector<std::string> m_parseCSVLine(const std::string& line);
    
};