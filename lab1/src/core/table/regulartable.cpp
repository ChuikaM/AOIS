#include <regulartable.hpp>
#include <tableloader.hpp>
#include <tablehelper.hpp>
#include <algorithm>

RegularTable::RegularTable(int size)
    : N(size)
{
    m_tableContent.records.reserve(N);
    m_tableContent.records.resize(N);
}

void RegularTable::loadTable(const std::string &filepath)
{
    auto loadedContent = TableLoader::loadFromFile(filepath, N);
    static int i = 0;
    for(auto record : loadedContent.records)
    {
        add(record, i++);
    }
    m_tableContent.titles = loadedContent.titles;
}

int RegularTable::indexOfFreeRecord() const
{
    auto tableRecords = m_tableContent.records;
    auto iter = std::find(tableRecords.begin(), tableRecords.end(), Record());
    if(iter == tableRecords.end()) 
        return -1;
    return std::distance(tableRecords.begin(), iter);
}

bool RegularTable::modify(const Record& record, int index)
{
    if(!TableHelper::indexValid(index, N)) return false;

    m_tableContent.records[index] = record;
    return true;
}

bool RegularTable::add(const Record &rec, int index)
{
    if(!TableHelper::canAdd(m_recordsCount, N)) return false;

    m_tableContent.records[index] = rec;
    m_recordsCount++;
    return true;
}

bool RegularTable::remove(const std::string &key, int index)
{
    if(!TableHelper::canRemove(m_recordsCount)) return false;
    if(!TableHelper::recordExistsAt(m_tableContent, key, index, N)) return false;

    m_tableContent.records[index] = Record();
    m_recordsCount--;
    return true;
}

bool RegularTable::find(const std::string& key, int index)
{
    if(!TableHelper::indexValid(index, N)) return false;

    return m_tableContent.records[index].fields[0] == key;
}

int RegularTable::indexOfRecord(const std::string &key)
{
    auto tableRecords = m_tableContent.records;
    auto iter = std::find_if(tableRecords.begin(), tableRecords.end(), [key](const Record& record){ return record.fields[0] == key; });
    if(iter == tableRecords.end()) 
        return -1;
    return std::distance(tableRecords.begin(), iter);
}

std::vector<Record> RegularTable::getData() const
{
    return m_tableContent.records;
}
std::vector<std::string> RegularTable::getTitles() const
{
    return m_tableContent.titles;
}
