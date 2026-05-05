#pragma once

class IReportPrinter {
public:
    virtual ~IReportPrinter() = default;

    virtual void printInfo() = 0;

};