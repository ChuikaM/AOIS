#pragma once
#include <inetwork.hpp>
#include <vector>

class BidirectionalAssociativeMemory : public INetwork {
public:
    explicit BidirectionalAssociativeMemory();

    void train(const Matrix<float>& trainData) override;
    
private:
    std::vector<float> m_w;

};