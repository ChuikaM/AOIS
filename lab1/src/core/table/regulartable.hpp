#pragma once
#include <itable.hpp>
#include <tableloader.hpp>

class RegularTable : public ITable
{
public:
    explicit RegularTable(int size);
    ~RegularTable() override = default;

    void loadTable(const std::string& filepath) override;

    bool modify(std::vector<std::string> values, int index) override;
    bool add(const Record& rec, int index) override;
    bool remove(const std::string& key, int index) override;
    bool find(const std::string& key, int index) override;

private:
    const int N;
    int m_recordsCount {};
    TableContent m_tableContent;
    
};