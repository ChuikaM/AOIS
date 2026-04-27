#pragma once
#include <vector>

class INetwork {
public:
    virtual ~INetwork() = default;

    virtual void train(std::vector<std::vector<int>> data) = 0;
    

};