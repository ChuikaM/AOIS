#pragma once
#include <inetwork.hpp>
#include <vector>

class HammingNetwork : public INetwork {
public:
    explicit HammingNetwork() = default;

    void train(const Matrix<float>& trainData) override;

    Matrix<float> sync(const Matrix<float>& noiseData, const Matrix<float>& originalData);

private:
    Matrix<float> m_w;
    Matrix<float> m_t;
};