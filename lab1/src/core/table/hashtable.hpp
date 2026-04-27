#pragma once
#include <itable.hpp>
#include <tableloader.hpp>

enum class ProbeMode { ADD, FIND, MODIFY, REMOVE };

class HashTable : public ITable
{
public:
    explicit HashTable(int size);
    ~HashTable() override = default;

    void loadTable(const std::string& filepath) override;
    
    int hashFunction(const std::string &key);

    bool add(const Record& rec, int index) override;
    TableResult find(const std::string& key, int index) override;
    bool modify(const Record& record, int index) override;
    bool remove(const std::string& key, int index) override;
    
    int indexOfRecord(const std::string& key, RecordMethod method) override;

    std::vector<Record> getData() const override;
    std::vector<std::string> getTitles() const override;

    int getTotalCollisions() const;

private:
    int m_linear_probing(int index, const std::string& key, ProbeMode mode);

    TableContent m_tableContent;
    const int N;
    int m_recordsCount {};
    int m_totalCollisions {};

};