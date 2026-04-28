#include <regulartable.hpp>
#include <tableloader.hpp>
#include <algorithm>

RegularTable::RegularTable(int size)
    : N(size)
    , m_tableHelper(m_tableContent, size)
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

bool RegularTable::modify(const Record& record, int index)
{
    if(!m_tableHelper.indexValid(index)) return false;

    m_tableContent.records[index] = record;
    return true;
}

bool RegularTable::add(const Record &record, int index)
{
    if (!m_tableHelper.indexValid(index)) return false;
    if(!m_tableHelper.canAdd(m_recordsCount)) return false;

    m_tableContent.records[index] = record;
    m_recordsCount++;
    return true;
}

bool RegularTable::remove(const std::string &key, int index)
{
    if(!m_tableHelper.canRemove(m_recordsCount)) return false;
    if(!m_tableHelper.recordExistsAt(key, index)) return false;

    m_tableContent.records[index] = Record();
    m_recordsCount--;
    return true;
}

TableResult RegularTable::find(const std::string& key, int index)
{
    if(!m_tableHelper.indexValid(index)) return {{}, false};

    return { m_tableContent.records[index], m_tableContent.records[index].fields[0] == key };
}

int RegularTable::indexOfRecord(const std::string &key, RecordMethod method)
{
    switch (method)
    {
        case RecordMethod::FREE_RECORD:
        {
            auto tableRecords = m_tableContent.records;
            auto iter = std::find(tableRecords.begin(), tableRecords.end(), Record());
            if(iter == tableRecords.end()) 
                return -1;
            return std::distance(tableRecords.begin(), iter);
        }
        case RecordMethod::INDEX_RECORD:
        {
            auto tableRecords = m_tableContent.records;
            auto iter = std::find_if(tableRecords.begin(), tableRecords.end(), [key](const Record& record){ return record.fields[0] == key; });
            if(iter == tableRecords.end()) 
                return -1;
            return std::distance(tableRecords.begin(), iter);
        }
    }

    return -1;
}

std::vector<Record> RegularTable::getData() const
{
    return m_tableContent.records;
}
std::vector<std::string> RegularTable::getTitles() const
{
    return m_tableContent.titles;
}
