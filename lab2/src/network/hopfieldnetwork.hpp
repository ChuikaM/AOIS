#pragma once
#include <inetwork.hpp>
#include <vector>

class HopfieldNetwork : public INetwork {
public:
    explicit HopfieldNetwork();

    void train(std::vector<std::vector<int>>) override;
    
private:
    std::vector<std::vector<int>> m_w;
    
};