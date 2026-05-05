#pragma once
#include <hammingnetwork.hpp>
#include <memory>
#include <vector>

class HammingManager {
public:
    explicit HammingManager(std::unique_ptr<HammingNetwork> network);

    void setOriginalData(std::vector<std::vector<float>> originalData);
    void setNoiseData(std::vector<std::vector<float>> noiseData);

    void printInfo();

private:
    std::unique_ptr<HammingNetwork> m_network;

    std::vector<std::vector<float>> m_originalData;
    std::vector<std::vector<float>> m_noiseData;

};