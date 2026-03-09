#include <txttable.hpp>

TxtTable::TxtTable(const std::string& filepath)
{
    Load(filepath);
    m_data = GetTableData();
}

bool TxtTable::Modify(const std::string& key, const std::string& newF2, const std::string& newF3)
{
    int idx = Find(key);
    if (idx != -1) {
        m_data[idx].field2 = newF2;
        m_data[idx].field3 = newF3;
        return true;  // Запись обновлена
    }
    return false;  // Запись не найдена
}

bool TxtTable::Add(const Record& rec)
{
    for (const auto& r : m_data) {
        if (r.key == rec.key) {
            return false; // Ошибка: Ключ уже существует
        }
    }
    m_data.push_back(rec);
    return true; // Запись добавлена
}

bool TxtTable::Delete(const std::string& key)
{
    int idx = Find(key);
    if (idx != -1) {
        m_data.erase(m_data.begin() + idx);
        return true; // Запись удалена
    }
    return false; // Запись не найдена
}

int TxtTable::Find(const std::string& key)
{
    for (size_t i = 0; i < m_data.size(); ++i) {
        if (m_data[i].key == key) return i;
    }
    return -1;
}
