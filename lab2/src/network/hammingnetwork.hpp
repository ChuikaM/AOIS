#pragma once
#include <inetwork.hpp>
#include <vector>
#include <hammingreport.hpp>

class HammingNetwork : public INetwork {
public:
    explicit HammingNetwork() = default;

    void train(const Matrix<float>& trainData) override;

    void sync(const Matrix<float>& noiseData, const Matrix<float>& originalData);

    HammingReport generateReport() const;

private:
    std::vector<float> compute_layer1(const std::vector<float> &noise);
    void compute_layer2(const std::vector<float> &y);

    Matrix<float> m_w;
    Matrix<float> m_t;

};