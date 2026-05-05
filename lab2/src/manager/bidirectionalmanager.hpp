#pragma once
#include <bidirectionalassociativememory.hpp>
#include <memory>
#include <vector>

class BidirectionalManager {
public:
    explicit BidirectionalManager(std::unique_ptr<BidirectionalAssociativeMemory> network);

    void setOriginalData(std::vector<std::vector<float>> x_originalData, std::vector<std::vector<float>> y_originalData);
    void setNoiseData(std::vector<std::vector<float>> x_noise, std::vector<std::vector<float>> y_noise);

    void printInfo();

private:
    std::unique_ptr<BidirectionalAssociativeMemory> m_network;

    std::vector<std::vector<float>> m_x_originalData;
    std::vector<std::vector<float>> m_x_noise;
    
    std::vector<std::vector<float>> m_y_originalData;
    std::vector<std::vector<float>> m_y_noise;

};