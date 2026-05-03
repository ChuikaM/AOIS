#include <hopfieldnetwork.hpp>

template <typename T> bool sgn(T val) {
    return val > T(0);
}

void HopfieldNetwork::train(const Matrix<float>& trainData)
{
  //  m_w.resize(trainData.column(), trainData.column());
    std::vector<float> dataOnes(trainData.column(), 1.0);
    Matrix<float> ones(dataOnes);
    
    for(size_t i = 0; i < trainData.row(); i++)
    {
        Matrix<float> trainDataMatrix(trainData.at_row(i));
        Matrix<float> w;
        w.resize(trainData.column(), trainData.column());
        w = (trainDataMatrix*2 - ones)*((trainDataMatrix*2 - ones).transpose()) - identity_matrix<float>(trainData.column(), 0.0);
        m_w.push_back(std::move(w));
    }
}

Matrix<float> HopfieldNetwork::async(const Matrix<float> &noiseData, const Matrix<float>& originalData)
{    
    for(size_t i = 0; i < m_w.size(); i++)
    {
        compute_async(noiseData.at_row(i), originalData.at_row(i), m_w[i]);
    }
    return {};
}

Matrix<float> HopfieldNetwork::sync(const Matrix<float> &noiseData, const Matrix<float>& originalData)
{
    for(size_t i = 0; i < m_w.size(); i++)
    {
        compute_sync(noiseData.at_row(i), originalData.at_row(i), m_w[i]);
    }
    return {};
}

void HopfieldNetwork::compute_async(const std::vector<float> &noiseData, const std::vector<float>& originalData, const Matrix<float>& w)
{
    std::vector<float> nextComputeData(noiseData);
    while(nextComputeData != originalData)
    {
        for(size_t i = 0; i < noiseData.size(); i++)
        {
            auto w_copy = w.at_column(i);
            float singValue = 0;
            for(int j = 0; j < noiseData.size(); j++)
            {
                singValue += w_copy[j] * nextComputeData[j];
            }
            nextComputeData[i] = sgn(singValue);
        }
    } 
}

void HopfieldNetwork::compute_sync(const std::vector<float> &noiseData, const std::vector<float>& originalData, const Matrix<float>& w)
{
    auto singMatrix = [](std::vector<float> &row){
        for(int i = 0; i < row.size(); i++)
        {
            row[i] = sgn(row[i]);
        }
    };
    
    std::vector<float> nextComputeData(noiseData);
    while(nextComputeData != originalData)
    {
        Matrix<float> y(nextComputeData);
        nextComputeData = (w * y).at_column(0);
        singMatrix(nextComputeData);
    }
}
