#pragma once
#include <inetwork.hpp>
#include <vector>
#include <bidirectionalreport.hpp>

enum class InputVariable : unsigned int { X, Y };

class BidirectionalAssociativeMemory : public INetwork {
public:
    explicit BidirectionalAssociativeMemory() = default;

    void train(const Matrix<float>& trainData) override;
    void train(const Matrix<float>& X, const Matrix<float>& Y);

    void sync(const Matrix<float>& noiseData, const Matrix<float>& originalData, InputVariable variable);

    BidirectionalReport generateReport() const;
    
private:
    Matrix<float> m_w;

};