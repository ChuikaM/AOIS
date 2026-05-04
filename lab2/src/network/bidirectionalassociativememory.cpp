#include <bidirectionalassociativememory.hpp>

void BidirectionalAssociativeMemory::train(const Matrix<float>& trainData)
{
   m_w.resize(trainData.column(), trainData.row());
    

}

void BidirectionalAssociativeMemory::train(const Matrix<float> &X, const Matrix<float> &Y)
{
   m_w = X.transpose() * Y;
}

void BidirectionalAssociativeMemory::sync(const Matrix<float> &noiseData, const Matrix<float> &originalData)
{
   
}
