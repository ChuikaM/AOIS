#pragma once
#include <tabledata.hpp>

class TableLoader
{
public:
    explicit TableLoader() = default;
    ~TableLoader() = default;

    static TableContent loadFromFile(const std::string& filepath, int N);
    
private:
    static std::vector<std::string> m_parseCSVLine(const std::string& line);
    
};