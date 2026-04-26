#pragma once
#include <tabledata.hpp>

class TableHelper {
public:
    explicit TableHelper() = default;
    ~TableHelper() = default;

    static bool indexValid(int index, int max);
    static bool canAdd(int recordCount, int max);
    static bool recordEmptyAt(TableContent content, int index, int max);
    static bool recordExistsAt(TableContent content, const std::string& key, int index, int max);

};