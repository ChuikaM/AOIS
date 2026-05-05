#include <hammingmanager.hpp>
#include <hammingreportprinter.hpp>
#include <helper.hpp>
#include <format>

HammingManager::HammingManager(std::unique_ptr<HammingNetwork> network)
    : m_network(std::move(network))
{}

void HammingManager::setOriginalData(std::vector<std::vector<float>> originalData)
{
    m_originalData = originalData;
}
void HammingManager::setNoiseData(std::vector<std::vector<float>> noiseData)
{
    m_noiseData = noiseData;
}

void HammingManager::printInfo()
{
    std::cout << "\nСеть Хэмминга:\n";

    std::cout << "\n1. Source vectors:\n";
    printSourceVectors(m_originalData);

    std::cout << "\n2. Example\n";
    m_network->sync(m_noiseData, m_originalData);
    auto report = m_network->generateReport();
    // HammingReportPrinter printer(report);
    // printer.printInfo();

    std::cout << "3. Maximum number of recognised noisy bits:\n";

}
