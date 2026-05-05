#include <memory>

#include <dataloader.hpp>
#include <dataconverter.hpp>

#include <network.hpp>
#include <manager.hpp>

int main()
{
    const int n = 2;
    const int m = 2;

    std::vector<std::vector<float>> originalData = DataLoader::loadFromFile("./file/original_data.csv");
    std::vector<std::vector<float>> noiseData = DataLoader::loadFromFile("./file/noise_data.csv");

    std::vector<std::vector<float>> X = DataConverter::splitData(originalData, n, true);
    std::vector<std::vector<float>> Y = DataConverter::splitData(originalData, m, false);
    std::vector<std::vector<float>> X_noise = DataConverter::splitData(noiseData, n, true);
    std::vector<std::vector<float>> Y_noise = DataConverter::splitData(noiseData, m, false);


    auto hopfieldNetwork = std::make_unique<HopfieldNetwork>();
    auto hammingNetwork = std::make_unique<HammingNetwork>();
    auto bidirectionalAssociativeMemory = std::make_unique<BidirectionalAssociativeMemory>();
    
    hopfieldNetwork->train(originalData);
    hammingNetwork->train(originalData);
    bidirectionalAssociativeMemory->train(X, Y);


    auto hopfieldManager = std::make_unique<HopfieldManager>(std::move(hopfieldNetwork));
    auto hammingManager = std::make_unique<HammingManager>(std::move(hammingNetwork));
    auto bidirectionalManager = std::make_unique<BidirectionalManager>(std::move(bidirectionalAssociativeMemory));

    hopfieldManager->setOriginalData(originalData);
    hammingManager->setOriginalData(originalData);
    bidirectionalManager->setOriginalData(X, Y);

    hopfieldManager->setNoiseData(noiseData);
    hammingManager->setNoiseData(noiseData);
    bidirectionalManager->setNoiseData(X_noise, Y_noise);

    hopfieldManager->printInfo();
    hammingManager->printInfo();
    bidirectionalManager->printInfo();

    return 0;
}