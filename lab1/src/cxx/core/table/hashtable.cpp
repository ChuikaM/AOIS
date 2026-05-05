#include <hashtable.hpp>
#include <iostream>
#include <limits>
#include <bit>

HashTable::HashTable(int size)
    : N(size),
    m_tableHelper(m_tableContent, size)
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
    unsigned short square = twoDigits * twoDigits;
    
    auto bits = std::bit_width(static_cast<unsigned int>(square));
    const auto MAX_BITS = static_cast<unsigned short>(log2(N));
    if(bits < MAX_BITS)
    {
        return square;
    }
    int shift = std::floor((bits - MAX_BITS) / 2);
    int mask = (1 << MAX_BITS) - 1;
    return static_cast<int>((square >> shift) & mask);
}

bool HashTable::add(const Record& record, int index)
{
    auto key = record.fields[0];
    if(!m_tableHelper.indexValid(index))
        return false;
    if(!m_tableHelper.canAdd(m_recordsCount))
        return false;

    if(hasCollisionAt(index))
        index = linearProbing(index, key, ProbeMode::ADD);
    if(!m_tableHelper.indexValid(index)) 
        return false;
    
    m_tableContent.records[index] = record;
    m_recordsCount++;
    return true;
}

TableResult HashTable::find(const std::string& key, int index)
{
    if(!m_tableHelper.indexValid(index)) 
        return {{}, false};
    
    if(hasCollisionAt(index))
        index = linearProbing(index, key, ProbeMode::FIND);
    if(!m_tableHelper.indexValid(index)) 
        return {{}, false};

    return { m_tableContent.records[index], m_tableHelper.recordExistsAt(key, index) };
}

bool HashTable::modify(const Record& record, int index)
{
    auto key = record.fields[0];
    if(!m_tableHelper.indexValid(index)) 
        return false;

    if(hasCollisionAt(index))
        index = linearProbing(index, key, ProbeMode::MODIFY);
    if(!m_tableHelper.indexValid(index)) 
        return false;
    if(!m_tableHelper.recordExistsAt(key, index))
        return false;

    m_tableContent.records[index] = record;
    return true;
}

bool HashTable::remove(const std::string &key, int index)
{
    if(!m_tableHelper.canRemove(m_recordsCount)) 
        return false;
    if(!m_tableHelper.indexValid(index)) 
        return false;

    if(hasCollisionAt(index))
        index = linearProbing(index, key, ProbeMode::REMOVE);

    if(!m_tableHelper.indexValid(index)) 
        return false;
    if(!m_tableHelper.recordExistsAt(key, index))
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

std::vector<int> HashTable::showStatistics()
{ 
    std::vector<int> statistics(N);
    auto data = getData();
    m_totalCollisions = 0;
    short i = 0;
    for(const auto record : data)
    {
        auto key = record.fields[0];
        if(!key.empty())
        {
            auto index = hashFunction(key);
            if(hasCollisionAt(index) && key != m_tableContent.records[index].fields[0]) 
            {
                m_totalCollisions++;
            }
            statistics[i++] = m_totalCollisions;
        }
    }
    return statistics;
}

bool HashTable::hasCollisionAt(int index)
{
    return !m_tableHelper.recordEmptyAt(index);
}

std::map<std::string, short> HashTable::countOccurrences()
{
    std::map<std::string, short> m;
    auto data = getData();
    for(const auto record : data)
    {
        auto key = record.fields[0];
        if(m.contains(key)) m[key]++;
        else m[key] = 1; 
    }
    return m;
}

int HashTable::linearProbing(int index, const std::string& key, ProbeMode mode)
{
    int startIndex = index;
    do {
        switch (mode)
        {
            case ProbeMode::ADD:
                if (!hasCollisionAt(index)) {
                    return index;
                }
                break;
            case ProbeMode::FIND:
            case ProbeMode::REMOVE:
            case ProbeMode::MODIFY:
                if (m_tableHelper.recordExistsAt(key, index)) {
                    return index;
                }
                break;
        }

        index = (index + 1) % N;
    } while (index != startIndex);

    return -1;
}
