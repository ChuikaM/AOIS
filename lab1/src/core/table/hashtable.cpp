#include <hashtable.hpp>
#include <tablehelper.hpp>
#include <iostream>
#include <limits>
#include <algorithm>

HashTable::HashTable(int size)
    : N(size)
{
    m_tableContent.records.reserve(N);
    m_tableContent.records.resize(N);
}

void HashTable::loadTable(const std::string &filepath)
{
    auto loadedContent = TableLoader::loadFromFile(filepath, N);
    for(auto record : loadedContent.records)
    {
        auto key = record.fields[0];
        int index = hashFunction(key);
        add(record, index);
    }
    m_tableContent.titles = loadedContent.titles;
}

int HashTable::hashFunction(const std::string &key)
{
    unsigned long long result = 1;
    auto max = std::numeric_limits<long long>::max();
    for (unsigned char c : key) 
    {
        result = (result * static_cast<int>(c)) % max;
    }
    auto twoDigits = static_cast<int>(result % 100);
    auto square = twoDigits * twoDigits;
    
    std::vector<int> possible_indexes {
        (square >> 1) & (N - 1),
        (square >> 7) & (N - 1),
        (square >> 4) & (N - 1)
    };
    return *(std::max_element(possible_indexes.begin(), possible_indexes.end()));
}

bool HashTable::add(const Record& rec, int index)
{
    if(!TableHelper::canAdd(m_recordsCount, N))
        return false;

    if(!TableHelper::recordEmptyAt(m_tableContent, index, N))
        index = m_linear_probing(index, rec.fields[0], ProbeMode::ADD);

    if(!TableHelper::indexValid(index, N)) 
        return false;
    
    m_tableContent.records[index] = rec;
    m_recordsCount++;
    return true;
}

TableResult HashTable::find(const std::string& key, int index)
{
    if(!TableHelper::indexValid(index, N)) 
        return {{}, false};
    
    index = m_linear_probing(index, key, ProbeMode::FIND);
    
    return { m_tableContent.records[index], index != -1 };
}

bool HashTable::modify(const Record& record, int index)
{
    if(!TableHelper::indexValid(index, N)) 
        return false;

    index = m_linear_probing(index, record.fields[0], ProbeMode::MODIFY);
    if(!TableHelper::indexValid(index, N)) 
        return false;

    m_tableContent.records[index] = record;
    return true;
}

bool HashTable::remove(const std::string &key, int index)
{
    if(!TableHelper::canRemove(m_recordsCount)) 
        return false;
    if(!TableHelper::indexValid(index, N)) 
        return false;

    index = m_linear_probing(index, key, ProbeMode::REMOVE);
    if(!TableHelper::indexValid(index, N)) 
        return false;

    m_tableContent.records[index] = {};
    m_recordsCount--;
    return true;
}

int HashTable::indexOfRecord(const std::string &key, RecordMethod method)
{
    switch (method)
    {
        case RecordMethod::FREE_RECORD:
        case RecordMethod::INDEX_RECORD: 
            return hashFunction(key);
    }
    return -1;
}

std::vector<Record> HashTable::getData() const
{
    return m_tableContent.records;
}
std::vector<std::string> HashTable::getTitles() const
{
    return m_tableContent.titles;
}

int HashTable::getTotalCollisions() const
{ 
    return m_totalCollisions;
}

int HashTable::m_linear_probing(int index, const std::string& key, ProbeMode mode)
{
    int startIndex = index;
    do {
        switch (mode)
        {
            case ProbeMode::ADD:
                if (TableHelper::recordEmptyAt(m_tableContent, index, N)) {
                    return index;
                }
                if (TableHelper::recordExistsAt(m_tableContent, key, index, N)) {
                    return -1;
                }
                break;
            case ProbeMode::FIND:
            case ProbeMode::REMOVE:
            case ProbeMode::MODIFY:
                if (TableHelper::recordExistsAt(m_tableContent, key, index, N)) {
                    return index;
                }
                break;
        }

        index = (index + 1) % N;
        ++m_totalCollisions;
    } while (index != startIndex);

    return -1;
}
