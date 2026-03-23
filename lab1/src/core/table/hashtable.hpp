#pragma once
#include <tablebase.hpp>

class HashTable : public TableBase
{
public:
    explicit HashTable(const std::string& filepath = "./files/file.csv");
    ~HashTable() override = default;
    
    bool Modify(std::vector<std::string> fieldsNew) override;
    bool Add(const Record& rec) override;
    bool Delete(const std::string& key) override;
    int Find(const std::string& key) override;
    int Collisions() const;

    std::vector<Record> GetData() const override;

private:
    int hashFunction(const std::string& key) const;
    
    static const int N = 64;
    std::vector<Record> m_data{N};

    int m_totalCollisions {};
    int m_count {};

};