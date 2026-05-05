#include <hopfieldnetwork.hpp>
#include <vector>
#include <cmath>

void HopfieldNetwork::train(const Matrix<float>& trainData)
{
    size_t n = trainData.column();
    size_t p = trainData.row();
    m_w.resize(n, n);

    for (size_t k = 0; k < p; ++k) {
        std::vector<float> y = trainData.at_row(k);
        std::vector<float> bip(n);
        for (size_t i = 0; i < n; ++i) bip[i] = 2.0f * y[i] - 1.0f;

        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                m_w.at(i, j) += bip[i] * bip[j];
    }

    for (size_t i = 0; i < n; ++i) m_w.at(i, i) = 0.0f;
}

void HopfieldNetwork::async(const std::vector<float>& noiseData, const std::vector<float>& originalData)
{
    std::vector<float> state = noiseData;
    int stage = 0;
    const int MAX_STAGES = 10;

    //report.iterations = originalData.size();
    while (stage < MAX_STAGES) {
        ++stage;
        std::vector<float> prev = state;

        for (size_t i = 0; i < state.size(); ++i) {
            float S = 0.0f;
            for (size_t j = 0; j < state.size(); ++j)
                S += m_w.at(i, j) * state[j];
            
            state[i] = (S > 0.0f) ? 1.0f : 0.0f;
        }

       // report.outputs.push_back(state);
        bool stable = (state == prev);
        // report.results.push_back(stable);
        // report.stages = stage;
        if (stable) break;
    }
}

void HopfieldNetwork::sync(const std::vector<float>& noiseData, const std::vector<float>& originalData)
{
    std::vector<float> state = noiseData;
    int stage = 0;
    const int MAX_STAGES = 10;

    //report.iterations = originalData.size();
    while (stage < MAX_STAGES) {
        ++stage;
        std::vector<float> prev = state;
        std::vector<float> next(state.size());

        for (size_t i = 0; i < state.size(); ++i) {
            float S = 0.0f;
            for (size_t j = 0; j < state.size(); ++j)
                S += m_w.at(i, j) * state[j];
            next[i] = (S > 0.0f) ? 1.0f : 0.0f;
        }

        state = next;
        //report.outputs.push_back(state);
        bool stable = (state == prev);
        // report.results.push_back(stable);
        // report.stages = stage;
        if (stable) break;
    }
}

HopfieldReport HopfieldNetwork::generateReport() const
{
    HopfieldReport report;

    return report;
}