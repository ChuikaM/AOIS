#pragma once
#include <hopfieldreport.hpp>
#include <ireportprinter.hpp>

class HopfieldReportPrinter : public IReportPrinter {
public:
    explicit HopfieldReportPrinter(const HopfieldReport& report);
    
    void printInfo() override;
    void printAsyncInfo();
    void printSyncInfo();

private:
    HopfieldReport m_report;

};