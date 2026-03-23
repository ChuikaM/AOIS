#include <hashtable.hpp>

HashTable::HashTable(const std::string& filepath)
{
    m_data.resize(N);
    load(filepath);
    for(const auto& rec : getTableData())
    {
        add(rec);
    }
}

bool HashTable::modify(std::vector<std::string> fieldsNew)
{
    auto key = fieldsNew[0];
    int index = find(key);
    if (index != -1) {
        for(size_t i = 0; i < fieldsNew.size(); i++)
        {
            m_data[index].fields[i] = fieldsNew[i];
        }
        return true;
    }
    return false;
}

bool HashTable::add(const Record& rec)
{
    if (rec.fields.empty() || m_count >= N) {
        return false;
    }

    std::string key = rec.fields[0];
    int index = m_hashFunction(key);
    int startIdx = index;
    bool collisionOccurred = false;
    int probes = 0;

    while (!m_data[index].isEmpty) {
        if (!m_data[index].isDeleted) {
            if (!m_data[index].fields.empty() && m_data[index].fields[0] == key) {
                return false;
            }
        }
        
        index = (index + 1) % N;
        collisionOccurred = true;
        
        if (++probes > N) {
            return false;
        }
    }

    if (collisionOccurred) {
        m_totalCollisions++;
    }

    m_data[index] = rec;
    m_data[index].isEmpty = false;
    m_data[index].isDeleted = false;
    
    ++m_count;
    return true;
}

bool HashTable::remove(const std::string& key)
{
    int index = find(key);
    if (index != -1) {
        m_data[index].isDeleted = true; 
        m_data[index].fields[0] = ""; 
        --m_count;
        return true;
    }
    return false;
}

int HashTable::find(const std::string& key)
{
    int index = m_hashFunction(key);
    int startIdx = index;

    while (!m_data[index].isEmpty) {
        auto keyLeft = m_data[index].fields[0];
        if (!m_data[index].isDeleted && keyLeft == key) {
            return index;
        }
        index = (index + 1) % N;
        if (index == startIdx) break;
    }
    return -1;
}

int HashTable::getTotalCollisions() const
{ 
    return m_totalCollisions;
}

std::vector<Record> HashTable::getData() const
{
    return m_data;
}

int HashTable::m_hashFunction(const std::string& key) const
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