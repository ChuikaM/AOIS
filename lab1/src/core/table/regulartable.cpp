#include <regulartable.hpp>

bool RegularTable::add(const Record& rec, int index)
{
    if(!TableBase::canAdd(rec, index)) return false;

    TableBase::addRecordAt(index, rec);
    return true;
}

bool RegularTable::find(const std::string& key, int index)
{
    return TableBase::recordExistsAt(index, key);
}