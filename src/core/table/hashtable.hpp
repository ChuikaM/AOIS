#pragma once
#include <tablebase.hpp>
#include <map>

class HashTable : public TableBase
{
public:
    explicit HashTable(const std::string& filepath = "./file/file.csv");
    ~HashTable() override = default;
    
    void Modify(const table::Cell& cell) override;
    void Add() override;
    void Delete() override;
    void Find() override;
    void Collisions();

private:
    void hashAdditiveSquared();
    void collisionWorker();
    
    std::vector<std::vector<std::string>> m_data;

};