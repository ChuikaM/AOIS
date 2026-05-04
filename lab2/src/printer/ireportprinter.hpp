#pragma once

class IReportPrinter {
public:
    ~IReportPrinter() = default;

    virtual void printInfo() = 0;

};