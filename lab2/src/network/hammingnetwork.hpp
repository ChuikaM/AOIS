#pragma once
#include <inetwork.hpp>
#include <vector>

class HammingNetwork : public INetwork {
public:
    explicit HammingNetwork();

    void train(const Matrix<float>& trainData) override;

    Matrix<float> sync(const Matrix<float>& data);

private:
    Matrix<float> m_w;
    float t {};
};