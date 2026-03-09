#pragma once
#include <tablebase.hpp>
#include <map>

class TxtTable : public TableBase
{
public:
    explicit TxtTable(const std::string& filepath = "./files/file.csv");
    ~TxtTable() override = default;
    
    bool Modify(const std::string& key, const std::string& newF2, const std::string& newF3) override;
    bool Add(const Record& rec) override;
    bool Delete(const std::string& key) override;
    int Find(const std::string& key) override;

    std::vector<Record> GetData() const;

private:
    std::vector<Record> m_data;
    
};