#pragma once
#include <itable.hpp>
#include <tableloader.hpp>
#include <tabledata.hpp>
#include <tablehelper.hpp>

class RegularTable : public ITable
{
public:
    explicit RegularTable(int size);
    ~RegularTable() override = default;

    void loadTable(const std::string& filepath) override;

    bool modify(const Record& record, int index) override;
    bool add(const Record& record, int index) override;
    bool remove(const std::string& key, int index) override;
    TableResult find(const std::string& key, int index) override;

    int indexOfRecord(const std::string& key, RecordMethod method) override;

    std::vector<Record> getData() const override;
    std::vector<std::string> getTitles() const override;

private:
    TableHelper m_tableHelper;

    const int N;
    int m_recordsCount {};
    TableContent m_tableContent;
    
};