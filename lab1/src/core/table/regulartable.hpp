#pragma once
#include <tablebase.hpp>

class RegularTable : public TableBase
{
public:
    explicit RegularTable() = default;
    ~RegularTable() override = default;

    bool add(const Record& rec) override;
    size_t find(const std::string& key) override;

};