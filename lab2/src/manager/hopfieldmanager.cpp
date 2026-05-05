#include <hopfieldmanager.hpp>
#include <hopfieldreportprinter.hpp>
#include <format>
#include <helper.hpp>

HopfieldManager::HopfieldManager(std::unique_ptr<HopfieldNetwork> network)
    : m_network(std::move(network))
{}

void HopfieldManager::setOriginalData(std::vector<std::vector<float>> originalData)
{
    m_originalData = originalData;
}
void HopfieldManager::setNoiseData(std::vector<std::vector<float>> noiseData)
{
    m_noiseData = noiseData;
}

void HopfieldManager::printInfo()
{
    std::cout << "\nСеть Хопфилда:\n";

    std::cout << "\n1. Source vectors:\n\n";
    printSourceVectors(m_originalData);

    std::cout << "\n2. Async example\n\n";
    async();

    std::cout << "\n3. Sync example\n\n";
    sync();

    std::cout << "\n4. Maximum number of recognised noisy bits:\n\n";

    std::cout << "Async:\n";
    for (size_t i = 0; i < m_originalData.size(); i++) 
    {
        m_network->async(m_noiseData[i], m_originalData[i]);
        auto report = m_network->generateReport();
        std::cout << std::format("y_{} = ", i+1) << report.max_errors << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Sync:\n";
    for (size_t i = 0; i < m_originalData.size(); i++) 
    {
        m_network->sync(m_noiseData[i], m_originalData[i]);
        auto report = m_network->generateReport();
        std::cout << std::format("y_{} = ", i+1) << report.max_errors << std::endl;
    }
    std::cout << std::endl;
    
}

void HopfieldManager::async()
{
    for (size_t i = 0; i < m_originalData.size(); i++) 
    {
        std::cout << std::format("for y{}\n", i);
        printVector(m_originalData[i], "y_original");

        m_network->async(m_noiseData[i], m_originalData[i]);
        auto report = m_network->generateReport();
        HopfieldReportPrinter printer(report);
        printer.printAsyncInfo();
    }
}
void HopfieldManager::sync()
{
    for (size_t i = 0; i < m_originalData.size(); i++) 
    {
        std::cout << std::format("for y{}\n", i);
        printVector(m_originalData[i], "y_original");
        
        m_network->sync(m_noiseData[i], m_originalData[i]);
        auto report = m_network->generateReport();
        HopfieldReportPrinter printer(report);
        printer.printSyncInfo();
    }
}
