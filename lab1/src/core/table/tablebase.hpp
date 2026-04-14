#pragma once
#include <itable.hpp>

class TableBase : public ITable
{
public:
    static const int N = 64;

    explicit TableBase();
    ~TableBase() override = default;

    void loadFromFile(const std::string& filepath = "./src/files/file.csv");
    
    bool modify(std::vector<std::string> fieldsNew, int index) override;
    bool remove(const std::string& key, int index) override;

    std::vector<std::string> getTitles() const override;
    std::vector<Record> getData() const override;

    bool canAdd(const Record& rec, int index) const;
    bool indexValid(int index) const;
    bool recordEmptyAt(int index) const;
    bool recordExistsAt(int index, const std::string& key) const;
    bool recordDeletedAt(int index) const;
    int indexOfFreeRecord() const;
    void addRecordAt(int index, const Record& rec);
    
private:
    std::vector<Record> m_load(const std::string& filepath);
    std::vector<std::string> m_parseCSVLine(const std::string& line);
 
    std::vector<std::string> m_titles;
    std::vector<Record> m_data;

    int m_count {};
    
};