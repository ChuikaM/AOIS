#include <tableloader.hpp>

#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

static std::string trim(const std::string& str) 
{
    const char* whitespace = " \t\r\n";
    auto start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}
TableContent TableLoader::loadFromFile(const std::string &filepath, int N)
{   
    namespace fs = std::filesystem;
    if (!fs::exists(filepath)) 
        throw std::runtime_error("File not found: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) 
        throw std::runtime_error("Cannot open file: " + filepath);

    std::vector<Record> records;
    std::vector<std::string> titles;
    
    std::string line;
    int lineNum = 0;
    bool titlesLoaded = false;
    while (std::getline(file, line)) 
    {
        if(lineNum > N) 
            break;
        ++lineNum;

        if (trim(line).empty()) 
            continue;
    
        if (!line.empty() && line.back() == '\r') 
            line.pop_back();
        
        auto fields = m_parseCSVLine(line);
        for (auto& field : fields) 
            field = trim(field);
        
        if (fields.size() != 3) 
        {
            throw std::runtime_error(
                "Line " + std::to_string(lineNum) + 
                " has " + std::to_string(fields.size()) + 
                " columns, expected 3"
            );
        }
        
        if (!titlesLoaded) 
        {
            titles = std::move(fields);
            titlesLoaded = true;
        } 
        else 
        {
            Record rec { fields };
            records.push_back(std::move(rec));
        }
    }
    file.close();
    return { records, titles };
}
std::vector<std::string> TableLoader::m_parseCSVLine(const std::string& line) 
{
    std::vector<std::string> fields;
    std::string field;
    auto inQuotes = false;
    for (int i = 0; i < line.size(); i++) 
    {
        char c = line[i];
        if (c == '"') 
        {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') 
            {
                field += '"';
                i++;
            } 
            else 
            {
                inQuotes = !inQuotes;
            }
        } 
        else if (c == ',' && !inQuotes) 
        {
            fields.push_back(field);
            field.clear();
        } 
        else 
        {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}