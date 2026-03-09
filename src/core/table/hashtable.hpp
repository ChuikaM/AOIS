#pragma once
#include <tablebase.hpp>
#include <map>

class HashTable : public TableBase
{
public:
    explicit HashTable(const std::string& filepath = "./file/file.csv");
    ~HashTable() override = default;
    
    bool Modify(const std::string& key, const std::string& newF2, const std::string& newF3) override;
    bool Add(const Record& rec) override;
    bool Delete(const std::string& key) override;
    int Find(const std::string& key) override;
    void Collisions();

private:
    int hashFunction(const std::string& key);
    void collisionWorker();
    
    static const int N = 64;
    std::vector<Record> m_data{N};
    std::vector<int> m_collisionHistory;

    int m_totalCollisions {};
    int m_count {};

};