#include <regulartable.hpp>

bool RegularTable::add(const Record& rec)
{
    if(!TableBase::canAdd(rec)) return false;

    int index = 0;
    TableBase::addRecordAt(index, rec);
    return true;
}

int RegularTable::find(const std::string& key)
{
    for (size_t i = 0; i < TableBase::getData().size(); i++) 
    {
        if(TableBase::recordExistsAt(i, key)) return i;
    }
    return -1;
}