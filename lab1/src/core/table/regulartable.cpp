#include <regulartable.hpp>

RegularTable::RegularTable(const std::string& filepath)
{
    Load(filepath);
    m_data = GetTableData();
}

bool RegularTable::Modify(std::vector<std::string> fieldsNew)
{
    auto key = fieldsNew[0];
    int index = Find(key);
    if (index != -1) {
        for(size_t i = 0; i < fieldsNew.size(); i++)
        {
            m_data[index].fields[i] = fieldsNew[i];
        }
        return true;
    }
    return false;
}

bool RegularTable::Add(const Record& rec)
{
    for (const auto& r : m_data) {
        auto keyLeft = r.fields[0];
        auto keyRight = rec.fields[0];
        if (keyLeft == keyRight) {
            return false;
        }
    }
    m_data.push_back(rec);
    return true;
}

bool RegularTable::Delete(const std::string& key)
{
    int idx = Find(key);
    if (idx != -1) {
        m_data.erase(m_data.begin() + idx);
        return true;
    }
    return false;
}

int RegularTable::Find(const std::string& key)
{
    for (size_t i = 0; i < m_data.size(); ++i) {
        auto keyLeft = m_data[i].fields[0];
        if (keyLeft == key) return i;
    }
    return -1;
}

std::vector<Record> RegularTable::GetData() const
{
    return m_data;
}
