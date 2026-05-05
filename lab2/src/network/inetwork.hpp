#pragma once
#include <matrix.h>
#include <vector>
#include <ireport.hpp>

class INetwork {
public:
    virtual ~INetwork() = default;

    virtual void train(const Matrix<float>& trainData) = 0;

};