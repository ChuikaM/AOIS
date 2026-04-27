#include <hammingnetwork.hpp>

HammingNetwork::HammingNetwork()
{
}

void HammingNetwork::train(const Matrix<float>& trainData)
{
    m_w.resize(trainData.column(), trainData.row());
    for(size_t i = 0; i < m_w.row(); i++)
    {
        for(size_t j = 0; j < m_w.column(); j++)
        {
           m_w.at(i, j) = pow(trainData.at(i, j), j) / 2;
        }
    }
    t = trainData.row() / 2;
}

Matrix<float> HammingNetwork::sync(const Matrix<float> &data)
{
    return Matrix<float>();
}
