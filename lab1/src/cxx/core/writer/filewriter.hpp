#pragma once
#include <vector>

class FileWriter {
public:
    explicit FileWriter() = default;
    static void write(const std::string& filename, const std::vector<int>& data);

};