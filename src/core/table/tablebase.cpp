#include <tablebase.hpp>

#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void TableBase::Load(const std::string& filepath) 
{   
    if (!fs::exists(filepath)) {
        throw std::runtime_error("File not found: " + filepath);
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    
    m_table_data.clear();
    m_titles.clear();
    
    std::string line;
    int lineNum = 0;
    bool titlesLoaded = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
    
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }
    
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        std::vector<std::string> fields = parseCSVLine(line);
        if (fields.size() != 3) {
            throw std::runtime_error(
                "Line " + std::to_string(lineNum) + 
                " has " + std::to_string(fields.size()) + 
                " columns, expected 3"
            );
        }
        
        if (!titlesLoaded) {
            m_titles = fields;
            titlesLoaded = true;
        } else {
            m_table_data.push_back(std::move(fields));
        }
    }
}
std::vector<std::string> TableBase::parseCSVLine(const std::string& line) 
{
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}