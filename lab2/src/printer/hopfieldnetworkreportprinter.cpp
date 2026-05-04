#include <hopfieldnetworkreportprinter.hpp>
#include <iostream>
#include <format>

void printVectorHighlighted(const std::vector<float>& data, const std::string& title, int highlightIndex)
{
    std::cout << std::format("{} ({}) = ", title, highlightIndex);

    std::cout << "[ ";
    for (size_t i = 0; i < data.size(); i++) {
        int val = static_cast<int>(data[i]);
        if (highlightIndex > 0 && i + 1 == static_cast<size_t>(highlightIndex))
            std::cout << "(" << val << ")";
        else
            std::cout << val;

        if (i + 1 < data.size())
            std::cout << " ";
    }
    std::cout << " ]";
    std::cout << std::endl;
}

HopfieldNetworkReportPrinter::HopfieldNetworkReportPrinter(const Report &report)
    : m_report(report)
{}

void HopfieldNetworkReportPrinter::printInfo()
{
    printAsyncInfo();
    printSyncInfo();
}

void HopfieldNetworkReportPrinter::printAsyncInfo()
{
    for(size_t i = 0; i < m_report.stages; i++)
    {
        std::cout << std::format("\nStage {}", i+1) << std::endl;
        for(size_t j = 0; j < m_report.iterations; j++)
        {
            printVectorHighlighted(m_report.outputs[i], "y_model", j+1);
        }
        if(m_report.results[i])
            std::cout << std::format("y_stage_{} == y_original -> relaxation, correct", i+1) << std::endl;
        else
            std::cout << std::format("y_stage_{} != y_original", i+1) << std::endl;
    }
}

void HopfieldNetworkReportPrinter::printSyncInfo()
{
    for(size_t i = 0; i < m_report.stages; i++)
    {
        std::cout << std::format("\nStage {}", i+1) << std::endl;

        std::cout << std::format("{} = ", "y_model", i+1);
        std::cout << "[ ";
        std::copy(m_report.outputs[i].begin(), m_report.outputs[i].end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << "]";
        std::cout << std::endl;

        if(m_report.results[i])
            std::cout << std::format("y_stage_{} == y_original -> relaxation, correct", i+1) << std::endl;
        else
            std::cout << std::format("y_stage_{} != y_original", i+1) << std::endl;
    }
}