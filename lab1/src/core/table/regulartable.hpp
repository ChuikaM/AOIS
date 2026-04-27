#pragma once
#include <itable.hpp>
#include <tableloader.hpp>
#include <tabledata.hpp>

class RegularTable : public ITable
{
public:
    explicit RegularTable(int size);
    ~RegularTable() override = default;

    void loadTable(const std::string& filepath) override;
    int indexOfFreeRecord() const;

    bool modify(const Record& record, int index) override;
    bool add(const Record& rec, int index) override;
    bool remove(const std::string& key, int index) override;
    TableResult find(const std::string& key, int index) override;

    int indexOfRecord(const std::string& key) override;

    std::vector<Record> getData() const override;
    std::vector<std::string> getTitles() const override;

private:
    const int N;
    int m_recordsCount {};
    TableContent m_tableContent;
    
};