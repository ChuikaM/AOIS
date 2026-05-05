#include <hopfieldreportprinter.hpp>
#include <iostream>
#include <format>
#include <helper.hpp>

HopfieldReportPrinter::HopfieldReportPrinter(const HopfieldReport &report)
    : m_report(report)
{}

void HopfieldReportPrinter::printInfo()
{
    printAsyncInfo();
    printSyncInfo();
}

void HopfieldReportPrinter::printAsyncInfo()
{
    for(size_t i = 0; i < m_report.stages; i++)
    {
        std::cout << std::format("\nStage {}", i+1) << std::endl;
        for(size_t j = 0; j < m_report.iterations; j++)
        {
            printVectorHighlighted(m_report.outputs[i], "y_model", j+1);
            if(m_report.bit_change_states[i][j])
                std::cout << "Bit changed value to " << m_report.outputs[i][j];
            std::cout << std::endl;
        }
        if(m_report.results[i])
            std::cout << std::format("y_stage_{} == y_original -> relaxation, correct", i+1) << std::endl;
        else
            std::cout << std::format("y_stage_{} != y_original", i+1) << std::endl;
    }
}

void HopfieldReportPrinter::printSyncInfo()
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