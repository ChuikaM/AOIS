#include <hashtable.hpp>

HashTable::HashTable(int size)
    : N(size)
{}

void HashTable::loadTable(const std::string &filepath)
{
    auto loadedContent = TableLoader::loadFromFile(filepath); // N
    for(auto record : loadedContent.records)
    {
        auto key = record.key;
        int index = hashFunction(key);
        add(record, index);
    }
}

int HashTable::hashFunction(const std::string &key)
{
    long long product = 1;
    const int MAX_LONG_NUMBER = 1000000000LL;
    for (unsigned char c : key) {
        product *= static_cast<int>(c);
        if (product > MAX_LONG_NUMBER) product %= MAX_LONG_NUMBER;
    }
    int twoDigits = static_cast<int>(product % 100);
    auto square = twoDigits * twoDigits;
    
    const int BITS = 6;
    const int SHIFT = (64 - BITS) / 2;
    int index = (square >> SHIFT) & ((1 << BITS) - 1);
    
    return index;
}

bool HashTable::add(const Record& rec, int index)
{
    if(!TableBase::canAdd(rec, index)) 
        return false;

    auto key = rec.key;
    m_tableContent.records[index] = rec;
    return true;
}

bool HashTable::find(const std::string& key, int index)
{
    auto startIdx = index;
    while (!TableBase::recordEmptyAt(index)) 
    {
        if(TableBase::recordExistsAt(index, key)) 
            return true;

        index = (index + 1) % N;
        if (index == startIdx) break;
    }
    return false;
}

bool HashTable::modify(std::vector<std::string> fieldsNew, int index)
{
    if(!indexValid(index)) 
        return false;

    auto key = fieldsNew[0];
    if(TableBase::recordExistsAt(index, key))
    {
        if (!m_linear_probing(index, key))
            return false;
    }
    return TableBase::modify(fieldsNew, index);
}

bool HashTable::remove(const std::string &key, int index)
{
    if(!indexValid(index)) 
        return false;

    if(TableBase::recordExistsAt(index, key))
    {
        if (!m_linear_probing(index, key))
            return false;
    }
    return TableBase::remove(key, index);
}

int HashTable::getTotalCollisions() const
{ 
    return m_totalCollisions;
}

bool HashTable::m_linear_probing(int& index, const std::string& key)
{
    int probes = 0;
    int startIdx = index;

    while (!TableBase::recordEmptyAt(index) && !TableBase::recordDeletedAt(index)) {
        if (TableBase::recordExistsAt(index, key)) {
            if (probes > 0)
                ++m_totalCollisions;
            return false;
        }
        index = (index + 1) % TableBase::N;
        ++probes;
        if (probes > TableBase::N || index == startIdx)
            return false;
    }

    if (probes > 0)
        m_totalCollisions += probes;

    return true;
}