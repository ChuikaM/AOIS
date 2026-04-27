#include <hopfieldnetwork.hpp>

HopfieldNetwork::HopfieldNetwork()
{
}

void HopfieldNetwork::train(const Matrix<float>& trainData)
{
    m_w.resize(trainData.column(), trainData.row());
    std::vector<std::vector<float>> dataOnes(
        trainData.row(), 
        std::vector<float>(trainData.column(), 1.0)
    );
    Matrix<float> ones(dataOnes);
    m_w = (trainData*2 - ones).transpose()*(trainData*2 - ones) - identity_matrix<float>(m_w.row(), 0.0);
}

Matrix<float> HopfieldNetwork::async(const Matrix<float> &data)
{
    
    return Matrix<float>();
}

Matrix<float> HopfieldNetwork::sync(const Matrix<float> &data)
{
    return Matrix<float>();
}
