#pragma once
#include <itable.hpp>

class TableBase : public ITable
{
public:
    static const size_t N = 64;

    explicit TableBase();
    ~TableBase() override = default;

    void loadFromFile(const std::string& filepath = "./src/files/file.csv");
    
    bool modify(std::vector<std::string> fieldsNew) override;
    bool remove(const std::string& key) override;

    std::vector<std::string> getTitles() const override;
    std::vector<Record> getData() const override;

    bool canAdd(const Record& rec) const;
    bool recordEmptyAt(size_t index) const;
    bool recordExistsAt(size_t index, const std::string& key) const;
    size_t indexOfFreeRecord() const;
    void addRecordAt(size_t index, const Record& rec);
    
private:
    std::vector<Record> m_load(const std::string& filepath);
    std::vector<std::string> m_parseCSVLine(const std::string& line);
 
    std::vector<std::string> m_titles;
    std::vector<Record> m_data;

    size_t m_count {};
    
};