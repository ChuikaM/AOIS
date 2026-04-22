#include <regulartable.hpp>
#include <tableloader.hpp>

RegularTable::RegularTable(int size)
    : N(size)
{}

void RegularTable::loadTable(const std::string &filepath)
{
    auto loadedContent = TableLoader::loadFromFile(filepath); // N
    static int i = 0;
    for(auto record : loadedContent.records)
    {
        add(record, i++);
    }
}

bool RegularTable::modify(std::vector<std::string> values, int index)
{
    if(!TableHelper::indexValid(index, N)) return false;
    m_tableContent.records[index].values = values;
    return true;
}

bool RegularTable::add(const Record &rec, int index)
{
    if(!TableBase::canAdd(rec, index)) return false;

    m_tableContent.records[index] = rec;
    m_recordsCount++;
    return true;
}

bool RegularTable::remove(const std::string &key, int index)
{
    if(!TableHelper::indexValid(index, N)) return false;
    m_tableContent.records.erase(m_tableContent.records.begin() + index);
    m_recordsCount--;
    return true;
}

bool RegularTable::find(const std::string& key, int index)
{
    if(!TableHelper::indexValid(index, N)) return false;
    return m_tableContent.records[index].key == key;
}