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
        return false; // Таблица переполнена
    }

    std::string key = rec.fields[0];  // Ключевое поле (строка)
    int index = hashFunction(key);
    int startIdx = index;
    bool collisionOccurred = false;

    // Линейное зондирование
    while (!m_data[index].isEmpty && !m_data[index].isDeleted) {
        // Проверка на дубликат ключа
        if (m_data[index].fields[0] == key && !m_data[index].isDeleted) {
            return false; // Ключ уже существует
        }
        
        index = (index + 1) % N;  // Циклический переход
        collisionOccurred = true;
        
        // Защита от зацикливания при полной таблице
        if (index == startIdx) {
            return false;
        }
    }

    // Фиксация коллизии
    if (collisionOccurred) {
        m_totalCollisions++;
    }

    // Вставка записи
    Record newRec = rec;
    newRec.isEmpty = false;
    newRec.isDeleted = false;
    newRec.viaCollision = collisionOccurred;
    
    m_data[index] = newRec;
    m_count++;
    
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

int HashTable::hashFunction(const std::string& key) const
{
    // Этап 1: Мультипликативный метод (все символы) → берём последние 2 цифры
    long long product = 1;
    for (unsigned char c : key) {
        product *= static_cast<int>(c);
        // Опционально: ограничиваем переполнение, но не обрезаем до 2 цифр на каждом шаге
        if (product > 1000000000LL) product %= 1000000000LL;
    }
    int twoDigits = static_cast<int>(product % 100);

    // Этап 2: Метод квадрата → извлекаем 6 средних битов для N=64
    long long square = static_cast<long long>(twoDigits) * twoDigits;
    
    // Для числа до 9801 (14 бит) средние 6 бит — это биты 4..9
    // Сдвигаем вправо на 4, затем маска 0x3F (6 бит)
    int hashIndex = (static_cast<int>(square) >> 4) & 0x3F;  // [0..63]
    
    return hashIndex;
}