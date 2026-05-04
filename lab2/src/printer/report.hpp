#pragma once
#include <vector>

struct Report
{
    std::vector<std::vector<float>> outputs;
    
    int max_errors {};
    int stages {};
    int iterations {};

    std::vector<float> results;

};
