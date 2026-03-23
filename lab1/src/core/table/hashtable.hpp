#pragma once
#include <tablebase.hpp>

class HashTable : public TableBase
{
public:
    explicit HashTable(const std::string& filepath = "./src/files/file.csv");
    ~HashTable() override = default;
    
    bool modify(std::vector<std::string> fieldsNew) override;
    bool add(const Record& rec) override;
    bool remove(const std::string& key) override;
    int find(const std::string& key) override;
    int getTotalCollisions() const;

    std::vector<Record> getData() const override;

private:
    int m_hashFunction(const std::string& key) const;
    
    static const int N = 64;
    std::vector<Record> m_data;

    int m_totalCollisions {};
    int m_count {};

};