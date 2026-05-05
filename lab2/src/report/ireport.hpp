#pragma once
#include <vector>

struct IReport
{
    int max_errors {};
    int stages {};
    int iterations {};
    
    std::vector<std::vector<float>> outputs;
    std::vector<float> results;

};
