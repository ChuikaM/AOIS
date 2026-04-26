#include <hashtable.hpp>
#include <tablehelper.hpp>
#include <bitset>

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
    long long result = 1;
    for (unsigned char c : key) {
        result *= static_cast<int>(c);
    }
    int twoDigits = static_cast<int>(result % 100);
    auto square = twoDigits * twoDigits;

    const int BITS = 6;
    const int SHIFT = (64 - BITS) / 2;
    int index = (square >> SHIFT) & ((1 << BITS) - 1);
    
    return index;
}

bool HashTable::add(const Record& rec, int index)
{
    if(!TableHelper::canAdd(m_recordsCount, N))
        return false;

    index = m_linear_probing(index);
    if(!TableHelper::indexValid(index, N)) 
        return false;
    
    m_tableContent.records[index] = rec;
    m_recordsCount++;
    return true;
}

bool HashTable::find(const std::string& key, int index)
{
    if(!TableHelper::indexValid(index, N)) 
        return false;
    
    while(!TableHelper::recordExistsAt(m_tableContent, key, index, N) && index != -1)
    {
        index = m_linear_probing(index);
    }
    return index != -1; // Неправильно проверет на сущестование эдемента с ключом
}

bool HashTable::modify(const Record& record, int index)
{
    if(!TableHelper::indexValid(index, N)) 
        return false;

    index = m_linear_probing(index);
    if(!TableHelper::indexValid(index, N)) 
        return false;

    m_tableContent.records[index] = record;
    return true;
}

bool HashTable::remove(const std::string &key, int index)
{
    if(!TableHelper::indexValid(index, N)) 
        return false;

    index = m_linear_probing(index);
    if(!TableHelper::indexValid(index, N)) 
        return false;

    m_tableContent.records[index] = {};
    m_recordsCount--;
    return true;
}

int HashTable::indexOfRecord(const std::string &key)
{
    return hashFunction(key);
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

int HashTable::m_linear_probing(int index)
{
    int startIndex = index;
    while(!TableHelper::recordEmptyAt(m_tableContent, index, N)) {
        index = (index + 1) % N;
        if(startIndex == index)
            return -1;

        ++m_totalCollisions;
    }
    return index;
}