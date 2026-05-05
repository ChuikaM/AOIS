#include <hammingnetwork.hpp>
#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>

void HammingNetwork::train(const Matrix<float>& trainData)
{
    size_t m = trainData.row();
    size_t n = trainData.column();

    m_w.resize(m, n);
    m_t.resize(1, m);

    for(size_t i = 0; i < m; i++)
    {
        m_t.at(0, i) = static_cast<float>(n) / 2.0f;
        for(size_t j = 0; j < n; j++)
        {
            m_w.at(i, j) = trainData.at(i, j) / 2.0f;
        }
    }
}

void HammingNetwork::sync(const Matrix<float>& noiseData, const Matrix<float>& originalData)
{
    std::vector<float> noise = noiseData.at_row(0);
    std::vector<float> y = compute_layer1(noise);
    compute_layer2(y);
}

HammingReport HammingNetwork::generateReport() const
{
    HammingReport report;
    
    return report;
}

std::vector<float> HammingNetwork::compute_layer1(const std::vector<float> &noise)
{
    size_t m = m_w.row();
    size_t n = noise.size();
    std::vector<float> y(m);

    for(size_t j = 0; j < m; j++)
    {
        float sum = m_t.at(0, j);
        for(size_t i = 0; i < n; i++)
        {
            sum += m_w.at(j, i) * noise[i];
        }
        y[j] = sum;
    }
    return y;
}

void HammingNetwork::compute_layer2(const std::vector<float> &y)
{
    const size_t m = y.size();
    //if (m == 0) return -1;

    float e = 0.5f / static_cast<float>(m);
    std::vector<float> z = y;
    const int MAX_ITER = 100;

    for(int iter = 0; iter < MAX_ITER; ++iter)
    {
        std::vector<float> next_z(m);
        int active_count = 0;

        for(size_t j = 0; j < m; ++j)
        {
            float sum_others = 0.0f;
            for(size_t k = 0; k < m; ++k)
            {
                if (k != j) sum_others += z[k];
            }
            float S = z[j] - e * sum_others;
            
            next_z[j] = (S > 0.0f) ? S : 0.0f;
            if (next_z[j] > 0.0f) ++active_count;
        }

        z = next_z;
        if (active_count <= 1) break;
    }

    auto win_iter = std::max_element(z.begin(), z.end());
   // if (*win_iter <= 0.0f) return -1;
    int index = static_cast<int>(std::distance(z.begin(), win_iter));
    
   // return report;
}