#pragma once
#include <ireport.hpp>
#include <vector>

struct HammingReport : public IReport {
    std::vector<std::vector<bool>> bit_change_states;
    
};