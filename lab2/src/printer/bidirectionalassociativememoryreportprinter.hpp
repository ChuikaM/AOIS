#pragma once
#include <report.hpp>
#include <ireportprinter.hpp>

class BidirectionalAssociativeMemoryReportPrinter : public IReportPrinter {
public:
    explicit BidirectionalAssociativeMemoryReportPrinter(const Report& report);

    void printInfo() override;

private:
    Report m_report;

};