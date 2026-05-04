#include <hopfieldnetwork.hpp>

template <typename T> bool sgn(T val) {
    return val > T(0);
}

void HopfieldNetwork::train(const Matrix<float>& trainData)
{
    m_w.resize(trainData.column(), trainData.column());
    std::vector<float> dataOnes(trainData.column(), 1.0);
    Matrix<float> ones(dataOnes);
    for(size_t i = 0; i < trainData.row(); i++)
    {
        Matrix<float> trainDataMatrix(trainData.at_row(i));
        m_w += (trainDataMatrix*2 - ones)*((trainDataMatrix*2 - ones).transpose());
    }
    m_w -= identity_matrix<float>(trainData.column(), 0.0)*trainData.column();
}

Report HopfieldNetwork::async(const std::vector<float> &noiseData, const std::vector<float>& originalData)
{    
    std::vector<float> nextComputeData(noiseData);
    Report report;
    report.iterations = noiseData.size();
    int stages = 0;
    while(nextComputeData != originalData && stages < 10)
    {
        stages++;
        for(size_t i = 0; i < noiseData.size(); i++)
        {
            auto w_copy = m_w.at_column(i);
            float singValue = 0;
            for(int j = 0; j < noiseData.size(); j++)
            {
                singValue += w_copy[j] * nextComputeData[j];
            }
            nextComputeData[i] = sgn(singValue);
            if(nextComputeData[i] != originalData[i])
                report.max_errors++;
        }

        report.outputs.push_back(nextComputeData);
        report.results.push_back(nextComputeData == originalData);
    } 
    report.stages = stages;
    return report;
}

Report HopfieldNetwork::sync(const std::vector<float> &noiseData, const std::vector<float>& originalData)
{
    auto singMatrix = [](std::vector<float> &row){
        for(int i = 0; i < row.size(); i++)
        {
            row[i] = sgn(row[i]);
        }
    };
    
    std::vector<float> nextComputeData(noiseData);
    Report report;
    int stages = 0;
    while(nextComputeData != originalData && stages < 10)
    {
        stages++;
        Matrix<float> y(nextComputeData);
        nextComputeData = (m_w * y).at_column(0);
        singMatrix(nextComputeData);

        if(nextComputeData != originalData)
                report.max_errors++;
        report.outputs.push_back(nextComputeData);
        report.results.push_back(nextComputeData == originalData);
    }
    report.stages = stages;
    report.iterations = stages;
    return report;
}