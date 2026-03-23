#include <tablebase.hpp>

#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

static std::string trim(const std::string& str) {
    const char* whitespace = " \t\r\n";
    auto start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

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
    
        if (trim(line).empty()) {
            continue;
        }
    
        // Убираем \r для совместимости с Windows
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        // Trim каждого поля после парсинга
        for (auto& field : fields) {
            field = trim(field);
        }
        
        if (fields.size() != 3) {
            throw std::runtime_error(
                "Line " + std::to_string(lineNum) + 
                " has " + std::to_string(fields.size()) + 
                " columns, expected 3"
            );
        }
        
        if (!titlesLoaded) {
            m_titles = fields; // Заголовки тоже триммируем
            titlesLoaded = true;
        } else {
            Record rec;
            for(const auto& field : fields)
            {
                rec.fields.push_back(field);
            }
            rec.isEmpty = false;
            rec.isDeleted = false;
            rec.viaCollision = false;
            m_table_data.push_back(std::move(rec));
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
            // Обработка экранированных кавычек: "" внутри кавычек
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                ++i; // пропускаем следующую кавычку
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
    fields.push_back(field); // добавляем последнее поле
    return fields;
}

std::vector<Record> TableBase::GetTableData() const
{
    return m_table_data;
}
Record TableBase::GetDataAt(int index) const
{
    return m_table_data[index];
}
std::vector<std::string> TableBase::GetTitles() const
{
    return m_titles;
}