#include <bidirectionalmanager.hpp>
#include <bidirectionalreportprinter.hpp>
#include <format>
#include <helper.hpp>

BidirectionalManager::BidirectionalManager(std::unique_ptr<BidirectionalAssociativeMemory> network)
    : m_network(std::move(network))
{}

void BidirectionalManager::setOriginalData(std::vector<std::vector<float>> x_originalData, std::vector<std::vector<float>> y_originalData)
{
    m_x_originalData = x_originalData;
    m_y_originalData = y_originalData;
}
void BidirectionalManager::setNoiseData(std::vector<std::vector<float>> x_noise, std::vector<std::vector<float>> y_noise)
{
    m_x_noise = x_noise;
    m_y_noise = y_noise;
}

void BidirectionalManager::printInfo()
{
    std::cout << "\nДвунаправленная ассоциативная память:\n\n";

    std::cout << "1. Source vectors:\n";
    printBidirectionalVectors(m_x_originalData, m_y_originalData);

    std::cout << "2. Example for x\n";
    m_network->sync(m_x_noise, m_x_originalData, InputVariable::X);
    auto report_x = m_network->generateReport();
    BidirectionalReportPrinter printer_x(report_x);
    printer_x.printInfo();

    std::cout << "3. Example for y\n";
    m_network->sync(m_y_noise, m_y_originalData, InputVariable::Y);
    auto report_y = m_network->generateReport();
    BidirectionalReportPrinter printer_y(report_y);
    printer_y.printInfo();

    std::cout << "4. Maximum number of recognised noisy bits:" << std::endl;

}
