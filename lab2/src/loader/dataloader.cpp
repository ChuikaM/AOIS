#include <dataloader.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>

// Вспомогательная функция обрезки пробелов (помещаем в анонимный namespace)
namespace {
    std::string trim(const std::string& str) {
        const char* whitespace = " \t\r\n";
        auto start = str.find_first_not_of(whitespace);
        if (start == std::string::npos) return "";
        auto end = str.find_last_not_of(whitespace);
        return str.substr(start, end - start + 1);
    }
}

std::vector<std::vector<float>> DataLoader::loadFromFile(const std::string &filepath) {
    std::vector<std::vector<float>> data;

    namespace fs = std::filesystem;
    if (!fs::exists(filepath)) 
        throw std::runtime_error("File not found: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) 
        throw std::runtime_error("Cannot open file: " + filepath);
    
    std::string line;
    while (std::getline(file, line)) {
        if (trim(line).empty()) 
            continue;
    
        if (!line.empty() && line.back() == '\r') 
            line.pop_back();
        
        auto fields = m_parseCSVLine(line);
        
        // Преобразуем строки в числа с плавающей точкой
        std::vector<float> row;
        row.reserve(fields.size());
        for (const auto& field : fields) {
            // Обрезаем пробелы вокруг поля (на случай " 1, 2, 3")
            std::string trimmedField = trim(field);
            if (!trimmedField.empty()) {
                try {
                    row.push_back(std::stof(trimmedField));
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to convert '" + trimmedField + "' to float in file " + filepath);
                }
            }
        }
        data.push_back(row);
    }
    file.close();
    return data;
}

std::vector<std::string> DataLoader::m_parseCSVLine(const std::string& line) {
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