#pragma once
#include <hammingreport.hpp>
#include <ireportprinter.hpp>

class HammingReportPrinter : IReportPrinter {
public:
    explicit HammingReportPrinter(const HammingReport& report);

    void printInfo() override;

private:
    HammingReport m_report;

};