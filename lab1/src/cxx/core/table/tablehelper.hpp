#pragma once
#include <tabledata.hpp>

class TableHelper {
public:
    explicit TableHelper(const TableContent& content, int max);
    ~TableHelper() = default;

    bool indexValid(int index);
    bool canAdd(int recordCount);
    bool canRemove(int recordCount);
    bool recordEmptyAt(int index);
    bool recordExistsAt(const std::string& key, int index);

private:
    const TableContent& m_tableContent;
    const int m_max;

};