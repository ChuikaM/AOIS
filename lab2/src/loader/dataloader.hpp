#pragma once
#include <vector>

class DataLoader {
public:
    explicit DataLoader() = default;

    static std::vector<std::vector<float>> loadFromFile(const std::string &filepath);

private:
    static std::vector<std::string> m_parseCSVLine(const std::string& line);

};