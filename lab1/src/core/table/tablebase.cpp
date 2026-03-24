#include <tablebase.hpp>

#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

TableBase::TableBase()
{
    m_data.resize(N);
    m_data.reserve(N);
}

static std::string trim(const std::string& str) {
    const char* whitespace = " \t\r\n";
    auto start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

std::vector<Record> TableBase::m_load(const std::string& filepath) 
{   
    namespace fs = std::filesystem;
    if (!fs::exists(filepath)) {
        throw std::runtime_error("File not found: " + filepath);
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    m_data.clear();
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
        
        std::vector<std::string> fields = m_parseCSVLine(line);
        
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
            m_data.push_back(std::move(rec));
        }
    }
    file.close();
    return m_data;
}

std::vector<std::string> TableBase::m_parseCSVLine(const std::string& line) 
{
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    
    for (int i = 0; i < line.size(); ++i) {
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

std::vector<std::string> TableBase::getTitles() const
{
    return m_titles;
}

void TableBase::loadFromFile(const std::string &filepath)
{
    auto table_data = m_load(filepath);
    for(const auto& rec : table_data)
    {
        add(rec);
    }
}

bool TableBase::modify(std::vector<std::string> fieldsNew)
{
    auto key = fieldsNew[0];
    int index = find(key);
    if (index != -1) {
        for(int i = 0; i < fieldsNew.size(); i++)
        {
            m_data[index].fields[i] = fieldsNew[i];
        }
        return true;
    }
    return false;
}

bool TableBase::remove(const std::string &key)
{
    int index = find(key);
    if (index != -1) {
        m_data[index].isEmpty = true; 
        m_data[index].isDeleted = true; 
        for(auto field : m_data[index].fields)
        {
            field = ""; 
        }
        --m_count;
        return true;
    }
    return false;
}

std::vector<Record> TableBase::getData() const
{
    return m_data;
}

bool TableBase::canAdd(const Record &rec) const
{
    return !rec.fields.empty() && m_count < N;
}
bool TableBase::recordEmptyAt(int index) const
{
    return m_data[index].isEmpty;
}
bool TableBase::recordDeletedAt(int index) const
{
    return m_data[index].isDeleted;
}
bool TableBase::recordExistsAt(int index, const std::string& key) const
{
    auto keyLeft = m_data[index].fields[0];
    return !m_data[index].isDeleted && keyLeft == key;
}
int TableBase::indexOfFreeRecord() const
{
    for(int i = 0; i < N; i++)
        if(m_data[i].isEmpty) return i;
    return -1;
}

void TableBase::addRecordAt(int index, const Record& rec)
{
    m_data[index] = rec;
    m_data[index].isEmpty = false;
    m_data[index].isDeleted = false;
    
    ++m_count;
}
