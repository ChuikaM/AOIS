#pragma once
#include <hopfieldnetwork.hpp>
#include <memory>
#include <vector>

class HopfieldManager {
public:
    explicit HopfieldManager(std::unique_ptr<HopfieldNetwork> network);

    void setOriginalData(std::vector<std::vector<float>> originalData);
    void setNoiseData(std::vector<std::vector<float>> noiseData);

    void printInfo();

private:
    void async();
    void sync();

    std::unique_ptr<HopfieldNetwork> m_network;

    std::vector<std::vector<float>> m_originalData;
    std::vector<std::vector<float>> m_noiseData;

};