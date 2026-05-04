#include <bidirectionalassociativememoryreportprinter.hpp>
#include <iostream>
#include <format>

BidirectionalAssociativeMemoryReportPrinter::BidirectionalAssociativeMemoryReportPrinter(const Report &report)
    : m_report(report)
{}

void BidirectionalAssociativeMemoryReportPrinter::printInfo()
{
    for(size_t i = 0; i < m_report.stages; i++)
    {
        std::cout << std::format("\nStage {}", i+1) << std::endl;
        for(size_t j = 0; j < m_report.iterations; j++)
        {
            //printVectorHighlighted(m_report.outputs[i], "y_model", j+1);
        }
        if(m_report.results[i])
            std::cout << std::format("y_stage_{} == y_original -> relaxation, correct", i+1) << std::endl;
        else
            std::cout << std::format("y_stage_{} != y_original", i+1) << std::endl;
    }
}