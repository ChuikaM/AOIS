#include <hammingnetwork.hpp>

template <typename T> bool F(T val) {
    return val > T(0) ? T : 0;
}

void HammingNetwork::train(const Matrix<float>& trainData)
{
    m_w.resize(trainData.column(), trainData.row());
    m_t.resize(1, trainData.column());

    for(size_t i = 0; i < m_w.row(); i++)
    {
        m_t.at(0, i) = trainData.column() / 2;
        for(size_t j = 0; j < m_w.column(); j++)
        {
           m_w.at(i, j) = pow(trainData.at(i, j), j) / 2;
        }
    }
}

Matrix<float> HammingNetwork::sync(const Matrix<float>& noiseData, const Matrix<float>& originalData)
{
    auto sum_at = [this](int i, const std::vector<float>& noise) -> float {
        float sum_y = 0;
        for(int j = 0; j < m_w.row(); j++)
        {
            sum_y += m_w.at(i, j) * noise[j];
        }
        return sum_y;
    };

    std::vector<float> noise = noiseData.at_row(0);
    std::vector<float> nextComputeData(noiseData);

    const int N = m_w.row();

    std::vector<float> y;
    for(int i = 0; i < m_w.row(); i++)
    {
        float sum_y = sum_at(i, noise);
        sum_y += N / 2;
        y.push_back(std::move(sum_y));
    }
    
    float e = floor(1 / N);
    std::vector<float> z(y);
    std::vector<float> s(y.size());
    while(std::count(s.begin(), s.end(), 0) < z.size() - 1)
    {
        for(int i = 0; i < y.size(); i++)
        {
            float sum_z = 0;
            for(int j = 0; j < z.size(); j++)
            {
                if(j == i) continue;
                sum_z += z[j];
            }
            s.push_back(z[i] - e * sum_z);
        }
        
        for(int j = 0; j < z.size(); j++)
        {
            z[j] = F(s[j]);
        }
    }
    return Matrix<float>();
}
