#include <hashtable.hpp>

HashTable::HashTable(const std::string& filepath)
{
    Load(filepath);
    m_data = GetTableData();
}

bool HashTable::Modify(const std::string& key, const std::string& newF2, const std::string& newF3)
{
    int index = Find(key);
    if (index != -1) {
        m_data[index].field2 = newF2;
        m_data[index].field3 = newF3;
        return true; // Запись успешно обновлена
    }
    return false; // Запись не найдена
}

bool HashTable::Add(const Record& rec)
{
    if (m_count >= N) {
        return false; // Ошибка: Хэш-таблица переполнена
    }

    int index = hashFunction(rec.key);
    int startIdx = index;
    int probes = 0;
    bool collisionOccurred = false;

    // Линейное зондирование
    while (!m_data[index].isEmpty && !m_data[index].isDeleted) {
        if (m_data[index].key == rec.key) {
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
        m_data[index].key = ""; 
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
        if (!m_data[index].isDeleted && m_data[index].key == key) {
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

int HashTable::hashFunction(const std::string& key)
{
    long long val = 1;
    for (char c : key) {
        val = (val * (int)c) % 100;
    }

    long long sq = val * val;
    return (sq >> 4) & 0x3F;
}
