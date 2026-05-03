#include <iostream>
#include <vector>

#include <network.hpp>
#include <memory>
#include <iterator>
#include <algorithm>
#include <format>

#include <matrix.h>

int main()
{
    const int n = 13;
    const int m = 13;
    std::vector<std::vector<float>> data = {
        {1, 1, 1, 0},
        {1, 1, 1, 0}
    };
    Matrix<float> trainData(data);

    auto printSourceVectors = [](std::vector<std::vector<float>> data){
        int i = 1;
        for(const auto row : data)
        {
            std::cout << std::format("y{}:", i++);
            std::cout << "[ ";
            std::copy(row.begin(), row.end(), std::ostream_iterator<int>(std::cout, " "));
            std::cout << "]";
            std::cout << "\n";
        }
    };

    std::cout << "\nСеть Хопфилда:\n\n";
    auto hopfieldNetwork = std::make_unique<HopfieldNetwork>();
    hopfieldNetwork->train(trainData);

    std::vector<std::vector<float>> noiseData = {
        {0, 0, 1, 0},
        {0, 0, 1, 0}
    };

    std::cout << "1. Source vectors:" << std::endl;
    printSourceVectors(data);
    std::cout << "2. Async example for y1 # need for every input vector" << std::endl;
    hopfieldNetwork->async(noiseData, data);
    std::cout << "3. Sync example for y1 # need for every input vector" << std::endl;
    hopfieldNetwork->sync(noiseData, data);
    std::cout << "4. Maximum number of recognised noisy bits:" << std::endl;


    std::cout << "\nСеть Хэмминга:\n\n";
    auto hammingNetwork = std::make_unique<HammingNetwork>();
    hammingNetwork->train(trainData);

    std::cout << "1. Source vectors:" << std::endl;
    printSourceVectors(data);
    std::cout << "2. Example for y1 # need for every input vector" << std::endl;
    hammingNetwork->sync(noiseData, data);
    std::cout << "3. Maximum number of recognised noisy bits:" << std::endl;


    std::cout << "\nДвунаправленная ассоциативная память:\n\n";
    auto bidirectionalAssociativeMemory = std::make_unique<BidirectionalAssociativeMemory>();
   // bidirectionalAssociativeMemory->train(trainData);

    std::cout << "1. Source vectors:" << std::endl;
    printSourceVectors(data);
    std::cout << "2. Example for x1 # need for every x-input vector" << std::endl;

    std::cout << "3. Example for y1 # need for every y-input vector" << std::endl;

    std::cout << "4. Maximum number of recognised noisy bits:" << std::endl;


    return 0;
}