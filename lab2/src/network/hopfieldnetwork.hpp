#pragma once
#include <inetwork.hpp>
#include <vector>

class HopfieldNetwork : public INetwork {
public:
    explicit HopfieldNetwork();

    void train(const Matrix<float>& trainData) override;
    Matrix<float> async(const Matrix<float>& data);
    Matrix<float> sync(const Matrix<float>& data);

private:
    Matrix<float> m_w;
    
};