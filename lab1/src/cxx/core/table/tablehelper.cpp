#include "tablehelper.hpp"

TableHelper::TableHelper(const TableContent &content, int size)
    : m_tableContent(content)
    , m_max(size)
{}

bool TableHelper::indexValid(int index)
{
    return index >= 0 && index < m_max;
}

bool TableHelper::canAdd(int recordCount)
{
    return recordCount < m_max;
}

bool TableHelper::canRemove(int recordCount)
{
    return recordCount >= 0;
}

bool TableHelper::recordEmptyAt(int index)
{
    if(!indexValid(index)) 
        return false;

    auto tableRecords = m_tableContent.records;
    return tableRecords[index] == Record();
}

bool TableHelper::recordExistsAt(const std::string &key, int index)
{
    if(!indexValid(index))
        return false;

    auto tableRecords = m_tableContent.records;
    return tableRecords[index].fields[0] == key;
}
