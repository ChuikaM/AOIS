#pragma once
#include <inetwork.hpp>
#include <vector>

class BidirectionalAssociativeMemory : public INetwork {
public:
    explicit BidirectionalAssociativeMemory();

    void train(std::vector<std::vector<int>> data) override;
    
private:
    std::vector<int> m_w;

};