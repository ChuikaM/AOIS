#pragma once
#include <ftxui/component/component.hpp>
#include <memory>

class TableBase;
class Record;

class App
{
public:
    App();
    ~App();
    
    void Run();

private:
    std::vector<std::string> m_form_inputs; 
    std::string m_search_key;
    std::unique_ptr<Record> m_found_record; 
    int m_found_index { -1 };
    bool m_operation_ok {};
    std::string m_feedback;
    
    std::unique_ptr<TableBase> regularTable;
    std::unique_ptr<TableBase> hashTable;
    
    int m_current_tab {};

};