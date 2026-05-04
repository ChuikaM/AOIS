#pragma once
#include <inetwork.hpp>
#include <vector>

class BidirectionalAssociativeMemory : public INetwork {
public:
    explicit BidirectionalAssociativeMemory() = default;

    void train(const Matrix<float>& trainData) override;
    void train(const Matrix<float>& X, const Matrix<float>& Y);

    void sync(const Matrix<float>& noiseData, const Matrix<float>& originalData);
    
private:
    Matrix<float> m_w;

};