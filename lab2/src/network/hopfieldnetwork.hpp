#pragma once
#include <inetwork.hpp>
#include <vector>

class HopfieldNetwork : public INetwork {
public:
    explicit HopfieldNetwork() = default;

    void train(const Matrix<float>& trainData) override;
    Matrix<float> async(const Matrix<float>& noiseData, const Matrix<float>& originalData);
    Matrix<float> sync(const Matrix<float> &noiseData, const Matrix<float>& originalData);

private:
    void compute_async(const std::vector<float> &noiseData, const std::vector<float>& originalData, const Matrix<float>& w);
    void compute_sync(const std::vector<float> &noiseData, const std::vector<float>& originalData, const Matrix<float>& w);

    std::vector<Matrix<float>> m_w;
    
};