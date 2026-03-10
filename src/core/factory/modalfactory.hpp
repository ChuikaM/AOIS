#pragma once
#include <ftxui/component/component.hpp>
#include <functional>
#include <string>
#include <vector>

enum class TableType {
    Simple,  // TxtTable
    Hash     // HashTable
};

enum class ModalAction {
    Print,
    Add,
    Edit,
    Delete,
    Find,
    Collisions
};

class TableBase;
class Record;

class ModalFactory {
public:
    // Контекст: внешние данные, которые нужны модалкам
    struct Context {
        // Для форм ввода
        std::vector<std::string>* input_fields;
        std::string* search_key;
        
        // Для отображения результата
        Record* result_record;
        int* result_index;
        
        // Флаги состояния
        bool* operation_success;
        std::string* feedback_message;
        
        // Колбэки для бизнес-логики (опционально)
        std::function<void()> on_refresh;
    };

    static ftxui::Component create(TableType type, 
                                   ModalAction action, 
                                   TableBase* table,
                                   Context ctx);
};