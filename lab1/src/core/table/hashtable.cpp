#include <hashtable.hpp>

bool HashTable::add(const Record& rec)
{
    if(!TableBase::canAdd(rec)) return false;

    std::string key = rec.fields[0];
    int index = m_hashFunction(key);

    bool result = linear_zondirovanie(index, key);
    if(!result) return false;

    TableBase::addRecordAt(index, rec);
    return true;
}

int HashTable::find(const std::string& key)
{
    int index = m_hashFunction(key);
    int startIdx = index;

    while (!TableBase::recordEmptyAt(index)) {
        if(TableBase::recordExistsAt(index, key)) return index;

        index = (index + 1) % N;
        if (index == startIdx) break;
    }
    return -1;
}

int HashTable::getTotalCollisions() const
{ 
    return m_totalCollisions;
}

bool HashTable::linear_zondirovanie(int index, std::string key)
{
    bool collisionOccurred = false;
    int probes = 0;

    while (!TableBase::recordEmptyAt(index)) {
        if(TableBase::recordExistsAt(index, key)) return false;
        
        index = (index + 1) % N;
        collisionOccurred = true;
        
        if (++probes > N) {
            return false;
        }
    }

    if (collisionOccurred) {
        m_totalCollisions++;
    }
    return true;
}

int HashTable::m_hashFunction(const std::string &key) const
{
    long long product = 1;
    for (unsigned char c : key) {
        product *= static_cast<int>(c);
        if (product > 1000000000LL) product %= 1000000000LL;
    }
    int twoDigits = static_cast<int>(product % 100);
    long long square = static_cast<long long>(twoDigits) * twoDigits;
    
    int hashIndex = (static_cast<int>(square) >> 4) & 0x3F;
    
    return hashIndex;
}