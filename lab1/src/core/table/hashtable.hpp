#pragma once
#include <itable.hpp>
#include <tableloader.hpp>
#include <tablehelper.hpp>
#include <map>

enum class ProbeMode : unsigned short { ADD, FIND, MODIFY, REMOVE };

class HashTable : public ITable
{
public:
    explicit HashTable(int size);
    ~HashTable() override = default;

    void loadTable(const std::string& filepath) override;
    
    int hashFunction(const std::string &key);

    bool add(const Record& record, int index) override;
    TableResult find(const std::string& key, int index) override;
    bool modify(const Record& record, int index) override;
    bool remove(const std::string& key, int index) override;
    
    int indexOfRecord(const std::string& key, RecordMethod method) override;

    std::vector<Record> getData() const override;
    std::vector<std::string> getTitles() const override;

    int showStatistics();
    std::map<std::string, short> countOccurrences();

private:
    int calculateCollisions();
    int linearProbing(int index, const std::string& key, ProbeMode mode);
    bool hasCollisionAt(int index);

    TableHelper m_tableHelper;
    TableContent m_tableContent;
    const int N;
    int m_recordsCount {};

};