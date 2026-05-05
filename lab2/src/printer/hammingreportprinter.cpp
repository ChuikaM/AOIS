#include <hammingreportprinter.hpp>
#include <iostream>
#include <format>
#include <helper.hpp>

HammingReportPrinter::HammingReportPrinter(const HammingReport &report)
    : m_report(report)
{}

void HammingReportPrinter::printInfo()
{
    for(size_t i = 0; i < m_report.iterations; i++)
    {
        printVector(m_report.outputs[i], std::format("winner({})", i+1));
        std::cout << std::endl;
    }
    //printVector(m_report.outputs[m_report.iterations], std::format("y_model({})", m_report.iterations));
    std::cout << std::endl;

    // if(m_report.results[0])
    //     std::cout << std::format("y_model({}) == y_original -> correct", m_report.iterations) << std::endl;
    // else
    //     std::cout << std::format("y_model({}) != y_original", m_report.iterations) << std::endl;

}
