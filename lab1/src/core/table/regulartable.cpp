#include <regulartable.hpp>

RegularTable::RegularTable(const std::string& filepath)
{
    m_data.reserve(N);
    load(filepath);
    for(const auto& rec : getTableData())
    {
        add(rec);
    }
}

bool RegularTable::modify(std::vector<std::string> fieldsNew)
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

bool RegularTable::add(const Record& rec)
{
    if(m_count >= N)
    {
        return false;
    }

    for (const auto& r : m_data) {
        auto keyLeft = r.fields[0];
        auto keyRight = rec.fields[0];
        if (keyLeft == keyRight) {
            return false;
        }
    }
    ++m_count;
    m_data.push_back(rec);
    return true;
}

bool RegularTable::remove(const std::string& key)
{
    int idx = find(key);
    if (idx != -1) {
        m_data.erase(m_data.begin() + idx);
        --m_count;
        return true;
    }
    return false;
}

int RegularTable::find(const std::string& key)
{
    for (size_t i = 0; i < m_data.size(); ++i) {
        auto keyLeft = m_data[i].fields[0];
        if (keyLeft == key) return i;
    }
    return -1;
}

std::vector<Record> RegularTable::getData() const
{
    return m_data;
}
