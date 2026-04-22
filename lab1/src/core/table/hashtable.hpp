#pragma once
#include <itable.hpp>
#include <tableloader.hpp>

class HashTable : public ITable
{
public:
    explicit HashTable(int size);
    ~HashTable() override = default;

    void loadTable(const std::string& filepath) override;
    
    int hashFunction(const std::string &key);

    bool add(const Record& rec, int index) override;
    bool find(const std::string& key, int index) override;
    bool modify(std::vector<std::string> fieldsNew, int index) override;
    bool remove(const std::string& key, int index) override;
    
    int getTotalCollisions() const;

private:
    bool m_linear_probing(int& index, const std::string& key);

    TableContent m_tableContent;
    const int N;
    int m_totalCollisions {};

};