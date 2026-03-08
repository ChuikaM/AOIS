#include <hashtable.hpp>

HashTable::HashTable(const std::string& filepath)
{
    Load(filepath);
    m_data = GetTableData();
}

void HashTable::Modify(const table::Cell &cell)
{

}

void HashTable::Add()
{
}

void HashTable::Delete()
{
}

void HashTable::Find()
{
}

void HashTable::Collisions()
{
}

void HashTable::hashAdditiveSquared()
{

}
