#pragma once
#include <inetwork.hpp>
#include <vector>

class BidirectionalAssociativeMemory : public INetwork {
public:
    explicit BidirectionalAssociativeMemory();

    void train() override;
    
private:
    std::vector<std::vector<int>> m_w;

};