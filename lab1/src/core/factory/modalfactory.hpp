#pragma once
#include <ftxui/component/component.hpp>
#include <functional>
#include <string>
#include <vector>

enum TableType {
    Regular,
    Hash
};

enum ModalAction {
    Print,
    Add,
    Edit,
    Delete,
    Find,
    Collisions
};

class ITable;
class Record;

int getIndexOfKey(ITable* table, const std::string& key);

class ModalFactory {
public:
    struct Context {
        std::vector<std::string>* input_fields;
        std::string* search_key;
        
        Record* result_record;
        int* result_index;
        
        bool* operation_success;
        std::string* feedback_message;
    
        std::function<void()> on_refresh;
    };

    ftxui::Component create(TableType type, 
                            ModalAction action, 
                            ITable* table,
                            Context ctx);

private:
    ftxui::Component m_createPrintView(ITable* table, ftxui::Color header_color);
    ftxui::Component m_createAddForm(ITable* table, ModalFactory::Context ctx);
    ftxui::Component m_createEditForm(ITable* table, ModalFactory::Context ctx);
    ftxui::Component m_createDeleteForm(ITable* table, ModalFactory::Context ctx);
    ftxui::Component m_createFindForm(ITable* table, ModalFactory::Context ctx);
    ftxui::Component m_createCollisionsView(ITable* table);

};