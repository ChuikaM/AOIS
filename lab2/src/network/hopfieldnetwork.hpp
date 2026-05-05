#pragma once
#include <inetwork.hpp>
#include <hopfieldreport.hpp>
#include <vector>

class HopfieldNetwork : public INetwork {
public:
    explicit HopfieldNetwork() = default;

    void train(const Matrix<float>& trainData) override;

    void async(const std::vector<float>& noiseData, const std::vector<float>& originalData);
    void sync(const std::vector<float> &noiseData, const std::vector<float>& originalData);

    HopfieldReport generateReport() const;

private:
    Matrix<float> m_w;
    
};