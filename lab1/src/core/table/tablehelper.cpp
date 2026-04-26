#include "tablehelper.hpp"

bool TableHelper::indexValid(int index, int max)
{
    return index >= 0 && index < max;
}

bool TableHelper::canAdd(int recordCount, int max)
{
    return recordCount < max;
}

bool TableHelper::recordEmptyAt(TableContent content, int index, int max)
{
    if(!indexValid(index, max)) 
        return false;

    auto tableRecords = content.records;
    return tableRecords[index] == Record();
}

bool TableHelper::recordExistsAt(TableContent content, const std::string &key, int index, int max)
{
    if(!indexValid(index, max)) 
        return false;

    auto tableRecords = content.records;
    return !recordEmptyAt(content, index, max) && tableRecords[index].fields[0] == key;
}
