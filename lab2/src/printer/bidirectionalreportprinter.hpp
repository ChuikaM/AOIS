#pragma once
#include <bidirectionalreport.hpp>
#include <ireportprinter.hpp>

class BidirectionalReportPrinter : public IReportPrinter {
public:
    explicit BidirectionalReportPrinter(const BidirectionalReport& report);

    void printInfo() override;

private:
    BidirectionalReport m_report;

};