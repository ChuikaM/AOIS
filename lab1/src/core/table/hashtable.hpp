#pragma once
#include <tablebase.hpp>

class HashTable : public TableBase
{
public:
    explicit HashTable() = default;
    ~HashTable() override = default;
    
    int hashFunction(const std::string &key);

    bool add(const Record& rec, int index) override;
    bool find(const std::string& key, int index) override;
    bool modify(std::vector<std::string> fieldsNew, int index) override;
    bool remove(const std::string& key, int index) override;
    
    int getTotalCollisions() const;

private:
    bool m_linear_probing(int& index, const std::string& key);

    int m_totalCollisions {};

};