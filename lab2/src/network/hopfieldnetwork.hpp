#pragma once
#include <inetwork.hpp>
#include <report.hpp>
#include <vector>

class HopfieldNetwork : public INetwork {
public:
    explicit HopfieldNetwork() = default;

    void train(const Matrix<float>& trainData) override;
    Report async(const std::vector<float>& noiseData, const std::vector<float>& originalData);
    Report sync(const std::vector<float> &noiseData, const std::vector<float>& originalData);

private:
    Matrix<float> m_w;
    
};