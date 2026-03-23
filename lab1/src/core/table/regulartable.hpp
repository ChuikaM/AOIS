#pragma once
#include <tablebase.hpp>

class RegularTable : public TableBase
{
public:
    explicit RegularTable(const std::string& filepath = "./src/files/file.csv");
    ~RegularTable() override = default;
    
    bool modify(std::vector<std::string> fieldsNew) override;
    bool add(const Record& rec) override;
    bool remove(const std::string& key) override;
    int find(const std::string& key) override;

    std::vector<Record> getData() const override;

private:
    static const int N = 64;
    std::vector<Record> m_data;
    
    int m_count {};
    
};