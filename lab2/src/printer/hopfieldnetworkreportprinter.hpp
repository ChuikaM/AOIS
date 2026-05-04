#pragma once
#include <report.hpp>

void printVectorHighlighted(const std::vector<float>& data, const std::string& title, int highlightIndex);

class HopfieldNetworkReportPrinter {
public:
    explicit HopfieldNetworkReportPrinter(const Report& report);

    void printAsyncInfo();
    void printSyncInfo();

private:
    Report m_report;

};