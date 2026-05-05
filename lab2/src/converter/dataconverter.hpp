#pragma once
#include <vector>

class DataConverter {
public:
    explicit DataConverter() = default;

    static std::vector<std::vector<float>> splitData(const std::vector<std::vector<float>>& data, int n, bool fromBegin);

};