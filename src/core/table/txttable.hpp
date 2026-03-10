#pragma once
#include <tablebase.hpp>
#include <map>

class TxtTable : public TableBase
{
public:
    explicit TxtTable(const std::string& filepath = "./files/file.csv");
    ~TxtTable() override = default;
    
    bool Modify(std::vector<std::string> fieldsNew) override;
    bool Add(const Record& rec) override;
    bool Delete(const std::string& key) override;
    int Find(const std::string& key) override;

    std::vector<Record> GetData() const override;

private:
    std::vector<Record> m_data;
    
};