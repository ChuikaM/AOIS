#include <regulartable.hpp>

bool RegularTable::add(const Record& rec)
{
    if(!TableBase::canAdd(rec)) return false;

    size_t index = TableBase::indexOfFreeRecord();
    if(index == -1) return false;
    
    TableBase::addRecordAt(index, rec);
    return true;
}

size_t RegularTable::find(const std::string& key)
{
    for (size_t i = 0; i < TableBase::getData().size(); i++) 
    {
        if(TableBase::recordExistsAt(i, key)) return i;
    }
    return -1;
}