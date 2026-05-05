#include <dataconverter.hpp>

std::vector<std::vector<float>> DataConverter::splitData(const std::vector<std::vector<float>> &data, int n, bool fromBegin)
{
    std::vector<std::vector<float>> new_data;
    for(const auto& row : data)
    {
        std::vector<float> new_row(fromBegin ? row.begin(), row.begin() + n : row.end() - n, row.end());
        new_data.push_back(new_row);
    }    
    return new_data;
}