#include <hashtable.hpp>

HashTable::HashTable(const std::string& filepath)
{
    Load(filepath);
    m_data = GetTableData();
}

bool HashTable::Modify(std::vector<std::string> fieldsNew)
{
    auto key = fieldsNew[0];
    int index = Find(key);
    if (index != -1) {
        for(size_t i = 0; i < fieldsNew.size(); i++)
        {
            m_data[index].fields[i] = fieldsNew[i];
        }
        return true; // Запись успешно обновлена
    }
    return false; // Запись не найдена
}

bool HashTable::Add(const Record& rec)
{
    if (m_count >= N) {
        return false; // Ошибка: Хэш-таблица переполнена
    }

    auto key = rec.fields[0];
    int index = hashFunction(key);
    int startIdx = index;
    int probes = 0;
    bool collisionOccurred = false;

    // Линейное зондирование
    while (!m_data[index].isEmpty && !m_data[index].isDeleted) {
        auto keyLeft = m_data[index].fields[0];
        auto keyRight = rec.fields[0];
        if (keyLeft == keyRight) {
            return false; // Ошибка: Ключ '" << rec.key << "' уже существует
        }
        index = (index + 1) % N;
        probes++;
        collisionOccurred = true;
        
        // Защита от бесконечного цикла (если таблица полна)
        if (index == startIdx) {
            return false; // Ошибка: Таблица переполнена (циклический поиск)
        }
    }

    if (collisionOccurred) {
        m_totalCollisions++;
    }

    Record newRec = rec;
    newRec.isEmpty = false;
    newRec.isDeleted = false;
    newRec.viaCollision = collisionOccurred;
    
    m_data[index] = newRec;
    m_count++;
    m_collisionHistory.push_back(m_totalCollisions);
    return true;
}

bool HashTable::Delete(const std::string& key)
{
    int accessCount;
    int index = Find(key);
    if (index != -1) {
        m_data[index].isDeleted = true;
        // Ключ очищаем для безопасности, но флаг isEmpty остается false
        m_data[index].fields[0] = ""; 
        m_count--;
        return true; // Запись успешно удалена
    }
    return false; // Запись не найдена
}

int HashTable::Find(const std::string& key)
{
    int index = hashFunction(key);
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

int HashTable::Collisions() const
{ 
    return m_totalCollisions;
}

std::vector<Record> HashTable::GetData() const
{
    return m_data;
}

int HashTable::hashFunction(const std::string& key)
{
    long long val = 1;
    for (char c : key) {
        val = (val * (int)c) % 100;
    }

    long long sq = val * val;
    return (sq >> 4) & 0x3F;
}
