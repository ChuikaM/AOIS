#pragma once
#include <inetwork.hpp>
#include <vector>
#include <report.hpp>

enum class InputVariable : unsigned int { X, Y };

class BidirectionalAssociativeMemory : public INetwork {
public:
    explicit BidirectionalAssociativeMemory() = default;

    void train(const Matrix<float>& trainData) override;
    void train(const Matrix<float>& X, const Matrix<float>& Y);

    Report sync(const Matrix<float>& noiseData, const Matrix<float>& originalData, InputVariable variable);
    
private:
    Report computeX(const Matrix<float>& noiseY, const Matrix<float>& originalY);
    Report computeY(const Matrix<float>& noiseX, const Matrix<float>& originalX);

    Matrix<float> m_w;

};