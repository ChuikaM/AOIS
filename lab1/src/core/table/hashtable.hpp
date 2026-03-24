#pragma once
#include <tablebase.hpp>

class HashTable : public TableBase
{
public:
    explicit HashTable() = default;
    ~HashTable() override = default;
    
    bool add(const Record& rec) override;
    int find(const std::string& key) override;
    int getTotalCollisions() const;

private:
    bool linear_probing(int& index, const std::string& key);
    int m_hashFunction(const std::string& key) const;

    int m_totalCollisions {};

};