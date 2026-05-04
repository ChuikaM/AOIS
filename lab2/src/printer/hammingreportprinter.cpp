#include <hammingreportprinter.hpp>
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

HammingReportPrinter::HammingReportPrinter(const Report &report)
    : m_report(report)
{}

void HammingReportPrinter::printInfo()
{
    for(size_t i = 0; i < m_report.iterations; i++)
    {
        std::cout << std::format("winner({}) = ", i+1);
        std::cout << "[ ";
        std::copy(m_report.outputs[i].begin(), m_report.outputs[i].end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << "]";
        std::cout << std::endl;
    }
    std::cout << std::format("y_model({}) = ", m_report.iterations);
    std::cout << "[ ";
    std::copy(m_report.outputs[m_report.iterations].begin(), m_report.outputs[m_report.iterations].end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "]";
    std::cout << std::endl;

    if(m_report.results[0])
        std::cout << std::format("y_model({}) == y_original -> correct", m_report.iterations) << std::endl;
    else
        std::cout << std::format("y_model({}) != y_original", m_report.iterations) << std::endl;

}
