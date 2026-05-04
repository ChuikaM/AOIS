#include <iostream>
#include <vector>

#include <memory>
#include <iterator>
#include <algorithm>
#include <format>

#include <network.hpp>
#include <hopfieldnetworkreportprinter.hpp>
#include <hammingreportprinter.hpp>
#include <bidirectionalassociativememoryreportprinter.hpp>

void printSourceVectors(const std::vector<std::vector<float>>& data);
void printVector(const std::vector<float>& data, const std::string& title);

void hopfieldnetwork_example(HopfieldNetwork* hopfieldNetwork, std::vector<std::vector<float>> originalData, std::vector<std::vector<float>> noiseData);
void hammingnetwork_example(HammingNetwork* hammingdNetwork, std::vector<std::vector<float>> originalData, std::vector<std::vector<float>> noiseData);
void bidirectionalassociativememory_example(BidirectionalAssociativeMemory* bidirectionalAssociativeMemory, std::vector<std::vector<float>> originalData, std::vector<std::vector<float>> noiseData);

int main()
{
    const int n = 13;
    const int m = 13;
    std::vector<std::vector<float>> data = {
        {1, 1, 1, 0},
        {1, 0, 0, 1}
    };
    std::vector<std::vector<float>> noiseData = {
        {0, 0, 1, 0},
        {1, 0, 1, 1}
    };
    auto hopfieldNetwork = std::make_unique<HopfieldNetwork>();
    auto hammingNetwork = std::make_unique<HammingNetwork>();
    auto bidirectionalAssociativeMemory = std::make_unique<BidirectionalAssociativeMemory>();
    
    hopfieldNetwork->train(data);
    hammingNetwork->train(data);
   // bidirectionalAssociativeMemory->train(data);

    hopfieldnetwork_example(hopfieldNetwork.get(), data, noiseData);

    // std::cout << "\nСеть Хэмминга:\n\n";
    // {
    //     std::cout << "1. Source vectors:" << std::endl;
    //     printSourceVectors(data);
    //     std::cout << "2. Example for y1 # need for every input vector" << std::endl;
    //     hammingNetwork->sync(noiseData, data);
    //     std::cout << "3. Maximum number of recognised noisy bits:" << std::endl;
    // }

    // std::cout << "\nДвунаправленная ассоциативная память:\n\n";
    // {
    //     std::cout << "1. Source vectors:" << std::endl;
    //     printSourceVectors(data);
    //     std::cout << "2. Example for x1 # need for every x-input vector" << std::endl;
    
    //     std::cout << "3. Example for y1 # need for every y-input vector" << std::endl;
    
    //     std::cout << "4. Maximum number of recognised noisy bits:" << std::endl;
    // }
    return 0;
}

void printSourceVectors(const std::vector<std::vector<float>>& data)
{
    int i = 1;
    for(const auto row : data)
    {
        printVector(row, std::format("y{}", i++));
    }
}
void printVector(const std::vector<float> &data, const std::string& title)
{
    std::cout << std::format("{} = ", title);
    std::cout << "[ ";
    std::copy(data.begin(), data.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "]";
    std::cout << std::endl;
}

void hopfieldnetwork_example(HopfieldNetwork *hopfieldNetwork, std::vector<std::vector<float>> originalData, std::vector<std::vector<float>> noiseData)
{
    auto run_example = [&](auto&& compute_report,
                          auto&& print_info) {
        for (size_t i = 0; i < originalData.size(); ++i) {
            std::cout << std::format("for y{}", i) << std::endl;
            printVector(originalData[i], "y_original");
            auto report = compute_report(noiseData[i], originalData[i]);
            HopfieldNetworkReportPrinter printer(report);
            print_info(printer);
        }
    };

    std::cout << "\nСеть Хопфилда:\n";
    {
        std::cout << "\n1. Source vectors:\n\n";
        printSourceVectors(originalData);

        std::cout << "\n2. Async example\n\n";
        run_example(
            [&](const auto& noise, const auto& orig) {
                return hopfieldNetwork->async(noise, orig);
            },
            [](auto& printer) { printer.printAsyncInfo(); }
        );

        std::cout << "\n3. Sync example\n\n";
        run_example(
            [&](const auto& noise, const auto& orig) {
                return hopfieldNetwork->sync(noise, orig);
            },
            [](auto& printer) { printer.printSyncInfo(); }
        );

        std::cout << "\n4. Maximum number of recognised noisy bits:\n\n";

        std::cout << "Async:\n";
        for (size_t i = 0; i < originalData.size(); i++) 
        {
            auto report = hopfieldNetwork->async(noiseData[i], originalData[i]);
            std::cout << std::format("y_{} = ", i+1) << report.max_errors << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Sync:" << std::endl;
        for (size_t i = 0; i < originalData.size(); i++) 
        {
            auto report = hopfieldNetwork->sync(noiseData[i], originalData[i]);
            std::cout << std::format("y_{} = ", i+1) << report.max_errors << std::endl;
        }
        std::cout << std::endl;
    }
}

void hammingnetwork_example(HammingNetwork *hammingdNetwork, std::vector<std::vector<float>> originalData, std::vector<std::vector<float>> noiseData)
{
    
}
