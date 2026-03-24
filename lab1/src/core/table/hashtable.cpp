#include <hashtable.hpp>

bool HashTable::add(const Record& rec)
{
    if(!TableBase::canAdd(rec)) return false;

    std::string key = rec.fields[0];
    size_t index = m_hashFunction(key);

    bool result = linear_probing(index, key);
    if(!result) return false;

    TableBase::addRecordAt(index, rec);
    return true;
}

size_t HashTable::find(const std::string& key)
{
    size_t index = m_hashFunction(key);
    size_t startIdx = index;

    while (!TableBase::recordEmptyAt(index)) {
        if(TableBase::recordExistsAt(index, key)) return index;

        index = (index + 1) % N;
        if (index == startIdx) break;
    }
    return -1;
}

size_t HashTable::getTotalCollisions() const
{ 
    return m_totalCollisions;
}

bool HashTable::linear_probing(int index, const std::string& key)
{
    int probes = 0;
    size_t startIdx = index;

    while (!TableBase::recordEmptyAt(index)) {
        if(TableBase::recordExistsAt(index, key)) 
            return false;
        
        index = (index + 1) % N;
        ++probes;
        
        if (probes > N || index == startIdx) 
            return false;
    }

    if (probes > 0) {
        m_totalCollisions += probes;
    }
    return true;
}

int HashTable::m_hashFunction(const std::string &key) const
{
    long long product = 1;
    for (unsigned char c : key) {
        product *= static_cast<size_t>(c);
        if (product > 1000000000LL) product %= 1000000000LL;
    }
    size_t twoDigits = static_cast<size_t>(product % 100);
    long long square = static_cast<long long>(twoDigits) * twoDigits;
    
    size_t hashIndex = (static_cast<size_t>(square) >> 4) & 0x3F;
    
    return hashIndex;
}