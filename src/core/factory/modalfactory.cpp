#include "modalfactory.hpp"
#include <ftxui/dom/table.hpp>
#include <algorithm>
#include <tablebase.hpp>
#include <hashtable.hpp>

using namespace ftxui;

static Component CreatePrintView(TableBase* table, Color header_color) {
    return Renderer([table, header_color] {
        auto data = table->GetData();
        auto titles = table->GetTitles();
        
        std::vector<std::vector<std::string>> entries = {titles};
        for (const auto& rec : data) {
            if (!rec.isEmpty) entries.push_back(rec.fields);
        }

        auto t = Table(entries);
        t.SelectAll().Border(LIGHT);
        t.SelectAll().SeparatorHorizontal(LIGHT);
        t.SelectAll().SeparatorVertical(LIGHT);
        t.SelectRow(0).Decorate(bold | color(header_color));
        
        return t.Render() | flex | frame | vscroll_indicator;
    });
}

static Component CreateAddForm(TableBase* table, ModalFactory::Context ctx) {
    auto titles = table->GetTitles();
    
    if (ctx.input_fields && ctx.input_fields->empty()) {
        ctx.input_fields->resize(titles.size());
        std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }

    std::vector<Component> rows;
    
    for (size_t i = 0; i < titles.size() && ctx.input_fields && i < ctx.input_fields->size(); ++i) {
        rows.push_back(Container::Horizontal({
            Renderer([title = titles[i]] { return text(title + ": "); }),
            Input(&(*ctx.input_fields)[i], "") | size(WIDTH, EQUAL, 25)
        }));
    }

    rows.push_back(Button("Добавить", [table, ctx] {
        if (!ctx.input_fields) return;
        bool all_filled = std::all_of(ctx.input_fields->begin(), ctx.input_fields->end(),
            [](const std::string& s) { return !s.empty(); });
        
        if (all_filled) {
            Record rec;
            rec.fields = *ctx.input_fields;
            rec.isEmpty = false;
            bool success = table->Add(rec);
            if (ctx.operation_success) *ctx.operation_success = success;
            if (ctx.feedback_message) {
                *ctx.feedback_message = success ? "✓ Добавлено" : "✗ Ошибка";
            }
            if (success) {
                std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
            }
        }
    }));

    if (ctx.feedback_message) {
        rows.push_back(Renderer([ctx] {
            if (ctx.feedback_message->empty()) return text("");
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? Color::Green : Color::Red;
            return text(*ctx.feedback_message) | bold | ftxui::color(color);
        }));
    }

    return Container::Vertical(std::move(rows)) | flex | frame;
}

// ===== Вспомогательная: форма редактирования =====
static Component CreateEditForm(TableBase* table, ModalFactory::Context ctx) {
    auto titles = table->GetTitles();
    
    // Инициализация полей (один раз при создании формы)
    if (ctx.input_fields && ctx.input_fields->empty()) {
        ctx.input_fields->resize(titles.size());
        std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }
    
    auto key_input = Input(ctx.search_key, "Ключ для поиска") | size(WIDTH, EQUAL, 20);
    
    auto find_btn = Button("Найти запись", [table, ctx] {
        if (!ctx.input_fields || !ctx.result_record || !ctx.search_key) return;
        
        int idx = table->Find(*ctx.search_key);
        if (idx >= 0 && ctx.result_index) {
            *ctx.result_index = idx;
            *ctx.result_record = table->GetDataAt(idx);
            if (ctx.result_record && !ctx.result_record->isEmpty) {
                *ctx.input_fields = ctx.result_record->fields;
            }
        }
    });

    // Кнопка "Сохранить" (создаётся заранее)
    auto save_btn = Button("Сохранить", [table, ctx] {
        if (!ctx.input_fields || !ctx.search_key) return;
        
        Record rec;
        rec.fields = *ctx.input_fields;
        rec.isEmpty = false;
        
        bool success = table->Modify({*ctx.input_fields});
        
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) {
            *ctx.feedback_message = success ? "✓ Сохранено" : "✗ Ошибка";
        }
        if (success && ctx.on_refresh) ctx.on_refresh();
    });

    return Renderer([ctx, titles, key_input, find_btn, save_btn] {
        std::vector<Element> rows;
        
        // Строка поиска: используем hbox с Element
        rows.push_back(hbox({
            text("Ключ: "),
            key_input->Render()  // Конвертируем Component в Element
        }));
        
        rows.push_back(find_btn->Render());

        // Поля редактирования (показываются только если запись найдена)
        bool found = (ctx.result_index && *ctx.result_index >= 0 && 
                     ctx.result_record && !ctx.result_record->isEmpty);
        
        if (found && ctx.input_fields) {
            for (size_t i = 0; i < titles.size() && i < ctx.input_fields->size(); ++i) {
                // Для каждой строки: label + input
                rows.push_back(hbox({
                    text(titles[i] + ": "),
                    Input(&(*ctx.input_fields)[i], "")->Render() | size(WIDTH, EQUAL, 25)
                }));
            }
            rows.push_back(save_btn->Render());
        }

        // Сообщение об успехе/ошибке
        if (ctx.feedback_message && !ctx.feedback_message->empty()) {
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? Color::Green : Color::Red;
            rows.push_back(text(*ctx.feedback_message) | bold | ftxui::color(color));
        }

        // ✅ Возвращаем Element: vbox из вектора Element + декораторы
        return vbox(rows) | flex | frame;
    });
}

// ===== Вспомогательная: удаление =====
static Component CreateDeleteForm(TableBase* table, ModalFactory::Context ctx) {
    auto key_title = table->GetTitles().empty() ? "Ключ" : table->GetTitles()[0];
    
    std::vector<Component> rows;
    
    rows.push_back(Container::Horizontal({
        Renderer([key_title] { return text(key_title + ": "); }),
        Input(ctx.search_key, "") | size(WIDTH, EQUAL, 30)  // ✅ Работает!
    }));
    
    rows.push_back(Button("Удалить", [table, ctx] {
        if (!ctx.search_key || ctx.search_key->empty()) return;
        bool success = table->Delete(*ctx.search_key);
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) {
            *ctx.feedback_message = success ? "✓ Удалено" : "✗ Не найдено";
        }
    }));
    
    if (ctx.feedback_message) {
        rows.push_back(Renderer([ctx] {
            if (ctx.feedback_message->empty()) return text("");
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? Color::Green : Color::Red;
            return text(*ctx.feedback_message) | bold | ftxui::color(color);
        }));
    }
    
    return Container::Vertical(std::move(rows)) | flex | frame;
}

// ===== Вспомогательная: поиск =====
static Component CreateFindForm(TableBase* table, ModalFactory::Context ctx) {
    return Renderer([table, ctx] {
        auto titles = table->GetTitles();
        if (titles.empty()) return text("Ошибка: нет заголовков");
        
        std::vector<Component> rows;
        
        // Строка поиска
        rows.push_back(Container::Horizontal({
            Renderer([title = titles[0]] { return text(title + ": "); }),
            Input(ctx.search_key, "Введите значение...") | size(WIDTH, EQUAL, 30)
        }));
        
        rows.push_back(Button("Найти", [table, ctx] {
            if (!ctx.search_key || !ctx.result_index || !ctx.result_record) return;
            
            *ctx.result_index = table->Find(*ctx.search_key);
            if (*ctx.result_index >= 0) {
                *ctx.result_record = table->GetDataAt(*ctx.result_index);
            }
        }));
        
        // Результат поиска
        rows.push_back(Renderer([ctx] {
            if (!ctx.result_index || !ctx.result_record) return text("");
            
            if (*ctx.result_index == -1 || ctx.result_record->isEmpty) {
                return text("🔍 Нет данных") | dim | center;
            }
            
            std::vector<Element> elements;
            for (size_t i = 0; i < ctx.result_record->fields.size(); ++i) {
                // Ключ выделяем цветом
                if (i == 0) {
                    elements.push_back(text(ctx.result_record->fields[i]) | bold | color(Color::Cyan));
                } else {
                    elements.push_back(text(ctx.result_record->fields[i]));
                }
                if (i < ctx.result_record->fields.size() - 1) {
                    elements.push_back(text(" | ") | dim);
                }
            }
            return hbox(elements) | borderLight;
        }));
        
        return vbox(std::move(rows)) | flex;
    });
}

// ===== Вспомогательная: коллизии (только HashTable) =====
static Component CreateCollisionsView(TableBase* table) {
    return Renderer([table] {
        // Предполагаем, что HashTable имеет метод Collisions()
        // Если нет — добавьте виртуальный метод в TableBase
        auto hash_table = dynamic_cast<HashTable*>(table);
        if (hash_table) {
            int count = hash_table->Collisions();
            return hbox({
                text("Коллизии: "),
                text(std::to_string(count)) | bold | color(Color::Magenta)
            }) | center | flex;
        }
        return text("Н/Д") | dim | center;
    });
}

// ===== ГЛАВНЫЙ МЕТОД ФАБРИКИ =====
Component ModalFactory::create(TableType type, ModalAction action, TableBase* table, Context ctx) {
    Color header_color = (type == TableType::Simple) ? Color::Cyan : Color::Green;
    
    switch (action) {
        case ModalAction::Print:
            return CreatePrintView(table, header_color);
            
        case ModalAction::Add:
            return CreateAddForm(table, ctx);
            
        case ModalAction::Edit:
            return CreateEditForm(table, ctx);
            
        case ModalAction::Delete:
            return CreateDeleteForm(table, ctx);
            
        case ModalAction::Find:
            return CreateFindForm(table, ctx);
            
        case ModalAction::Collisions:
            // ✅ ОБА варианта возвращают Component
            return (type == TableType::Hash) 
                ? CreateCollisionsView(table) 
                : Renderer([] { 
                    return text("Только для HashTable") | dim;  // ✅ return + Element
                });
            
        default:
            // ✅ Renderer с правильным возвратом Element
            return Renderer([] { 
                return text("Неизвестное действие") | dim; 
            });
    }
}