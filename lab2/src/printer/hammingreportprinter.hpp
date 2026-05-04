#pragma once
#include <report.hpp>
#include <ireportprinter.hpp>

void printVectorHighlighted(const std::vector<float>& data, const std::string& title, int highlightIndex);

class HammingReportPrinter : IReportPrinter {
public:
    explicit HammingReportPrinter(const Report& report);

    void printInfo() override;

private:
    Report m_report;

};