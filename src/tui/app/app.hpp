#pragma once
#include <ftxui/component/component.hpp>
#include <memory>

class HashTable;
class TxtTable;
class Record;

class App
{
public:
    App();
    ~App();
    
    void Run();

private:
    ftxui::Component ModalSimpleTablePrint();
    ftxui::Component ModalSimpleTableAdd();
    ftxui::Component ModalSimpleTableEdit();
    ftxui::Component ModalSimpleTableDelete();
    ftxui::Component ModalSimpleTableFind();

    ftxui::Component ModalHashTablePrint();
    ftxui::Component ModalHashTableAdd();
    ftxui::Component ModalHashTableEdit();
    ftxui::Component ModalHashTableDelete();
    ftxui::Component ModalHashTableFind();
    ftxui::Component ModalHashTableCollisions();

    int m_current_tab {};

    std::unique_ptr<HashTable> hashTable;
    std::unique_ptr<TxtTable> txtTable;

    std::vector<std::string> m_titles_content; 
    std::vector<std::string> m_dropdown_titles;
    int m_dropdown_selected {};
    std::string m_content;

    bool m_row_added {};
    bool m_modified {};
    bool m_deleted {};
    int m_index {};
    std::unique_ptr<Record> m_record;



};