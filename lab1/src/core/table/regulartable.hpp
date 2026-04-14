#pragma once
#include <tablebase.hpp>

class RegularTable : public TableBase
{
public:
    explicit RegularTable() = default;
    ~RegularTable() override = default;

    bool add(const Record& rec, int index) override;
    bool find(const std::string& key, int index) override;

};