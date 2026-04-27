#pragma once
#include <ftxui/component/component.hpp>
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
    Collisions,
    Graph
};

class ITable;
class Record;

class ModalFactory {
public:
    struct Context {
        std::vector<std::string>* input_fields;
        std::string* search_key;
        
        bool* operation_success;
        std::string* feedback_message;
    };

    ftxui::Component create(TableType type, ModalAction action, ITable* table, Context ctx);

private:
    ftxui::Component createFeedbackRenderer(Context ctx) const;
    std::vector<ftxui::Component> createInputRows(const std::vector<std::string>& titles, Context ctx, int width = 25) const;
    ftxui::Component createKeyInputRow(const std::string& label, Context ctx) const;

    ftxui::Component m_createPrintView(ITable* table, ftxui::Color header_color);
    ftxui::Component m_createAddForm(ITable* table, ModalFactory::Context ctx);
    ftxui::Component m_createEditForm(ITable* table, ModalFactory::Context ctx);
    ftxui::Component m_createDeleteForm(ITable* table, ModalFactory::Context ctx);
    ftxui::Component m_createFindForm(ITable* table, ModalFactory::Context ctx);
    ftxui::Component m_createCollisionsView(ITable* table);
    ftxui::Component m_createGraphView(ITable* table);

};