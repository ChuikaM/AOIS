#include <hammingnetwork.hpp>

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

int HammingNetwork::sync(const Matrix<float>& noiseData, const Matrix<float>& originalData)
{
    std::vector<float> noise = noiseData.at_row(0);

    auto y = compute_layer1(noise);
    auto index = compute_layer2(y);

    return index;
}

std::vector<float> HammingNetwork::compute_layer1(const std::vector<float> &noise)
{
    auto sum_at = [this](int i, const std::vector<float>& noise) -> float {
        float sum_y = 0;
        for(int j = 0; j < m_w.row(); j++)
        {
            sum_y += m_w.at(i, j) * noise[j];
        }
        return sum_y;
    };

    const int N = m_w.row();
    std::vector<float> y;
    for(size_t i = 0; i < m_w.row(); i++)
    {
        float sum_y = sum_at(i, noise);
        sum_y += N / 2;
        y.push_back(std::move(sum_y));
    }
    return y;
}

int HammingNetwork::compute_layer2(const std::vector<float> &y)
{
    auto F = []<typename T>(T val) -> T {
        return val > T(0) ? val : T(0);
    };

    const int N = m_w.row();
    float e = floor(1 / N);
    std::vector<float> z(y);
    std::vector<float> s(y.size());
    while(std::count(s.begin(), s.end(), 0) < z.size() - 1)
    {
        for(size_t i = 0; i < y.size(); i++)
        {
            float sum_z = 0;
            for(size_t j = 0; j < z.size(); j++)
            {
                if(j == i) continue;
                sum_z += z[j];
            }
            s.push_back(z[i] - e * sum_z);
        }
        
        for(size_t j = 0; j < z.size(); j++)
        {
            z[j] = F(s[j]);
        }
    }

    auto iter = std::find_if(s.begin(), s.end(), [](const float val){return val > 0;});
    if(iter == s.end()) 
        return -1;
    return std::distance(s.begin(), iter);
}
