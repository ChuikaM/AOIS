#include <modalfactory.hpp>

#include <ftxui/dom/table.hpp>
#include <algorithm>

#include <tablebase.hpp>
#include <hashtable.hpp>

static ftxui::Component CreatePrintView(TableBase* table, ftxui::Color header_color) {
    return ftxui::Renderer([table, header_color] {
        auto data = table->GetData();
        auto titles = table->GetTitles();
        
        std::vector<std::vector<std::string>> entries = {titles};
        for (const auto& rec : data) {
            if (!rec.isEmpty) entries.push_back(rec.fields);
        }

        auto t = ftxui::Table(entries);
        t.SelectAll().Border(ftxui::LIGHT);
        t.SelectAll().SeparatorHorizontal(ftxui::LIGHT);
        t.SelectAll().SeparatorVertical(ftxui::LIGHT);
        t.SelectRow(0).Decorate(ftxui::bold | ftxui::color(header_color));
        
        return t.Render() | ftxui::flex | ftxui::frame | ftxui::vscroll_indicator;
    });
}

static ftxui::Component CreateAddForm(TableBase* table, ModalFactory::Context ctx) {
    auto titles = table->GetTitles();
    
    if (ctx.input_fields && ctx.input_fields->empty()) {
        ctx.input_fields->resize(titles.size());
        std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }

    std::vector<ftxui::Component> rows;
    
    for (size_t i = 0; i < titles.size() && ctx.input_fields && i < ctx.input_fields->size(); ++i) {
        rows.push_back(ftxui::Container::Horizontal({
            ftxui::Renderer([title = titles[i]] { return ftxui::text(title + ": "); }),
            ftxui::Input(&(*ctx.input_fields)[i], "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 25)
        }));
    }

    rows.push_back(ftxui::Button("Добавить", [table, ctx] {
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
        rows.push_back(ftxui::Renderer([ctx] {
            if (ctx.feedback_message->empty()) return ftxui::text("");
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? ftxui::Color::Green : ftxui::Color::Red;
            return ftxui::text(*ctx.feedback_message) | ftxui::bold | ftxui::color(color);
        }));
    }

    return ftxui::Container::Vertical(std::move(rows)) | ftxui::flex | ftxui::frame;
}

static ftxui::Component CreateEditForm(TableBase* table, ModalFactory::Context ctx) {
    auto titles = table->GetTitles();
    
    if (ctx.input_fields && ctx.input_fields->empty()) {
        ctx.input_fields->resize(titles.size());
        std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }
    
    auto key_input = ftxui::Input(ctx.search_key, "Ключ для поиска") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20);
    
    auto find_btn = ftxui::Button("Найти запись", [table, ctx] {
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

    auto save_btn = ftxui::Button("Сохранить", [table, ctx] {
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

    return ftxui::Renderer([ctx, titles, key_input, find_btn, save_btn] {
        std::vector<ftxui::Element> rows;
        
        rows.push_back(ftxui::hbox({
            ftxui::text("Ключ: "),
            key_input->Render()
        }));
        
        rows.push_back(find_btn->Render());

        bool found = (ctx.result_index && *ctx.result_index >= 0 && 
                     ctx.result_record && !ctx.result_record->isEmpty);
        
        if (found && ctx.input_fields) {
            for (size_t i = 0; i < titles.size() && i < ctx.input_fields->size(); ++i) {
                rows.push_back(ftxui::hbox({
                    ftxui::text(titles[i] + ": "),
                    ftxui::Input(&(*ctx.input_fields)[i], "")->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 25)
                }));
            }
            rows.push_back(save_btn->Render());
        }

        if (ctx.feedback_message && !ctx.feedback_message->empty()) {
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? ftxui::Color::Green : ftxui::Color::Red;
            rows.push_back(ftxui::text(*ctx.feedback_message) | ftxui::bold | ftxui::color(color));
        }

        return vbox(rows) | ftxui::flex | ftxui::frame;
    });
}

static ftxui::Component CreateDeleteForm(TableBase* table, ModalFactory::Context ctx) {
    auto key_title = table->GetTitles().empty() ? "Ключ" : table->GetTitles()[0];
    
    std::vector<ftxui::Component> rows;
    
    rows.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([key_title] { return ftxui::text(key_title + ": "); }),
        ftxui::Input(ctx.search_key, "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)
    }));
    
    rows.push_back(ftxui::Button("Удалить", [table, ctx] {
        if (!ctx.search_key || ctx.search_key->empty()) return;
        bool success = table->Delete(*ctx.search_key);
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) {
            *ctx.feedback_message = success ? "✓ Удалено" : "✗ Не найдено";
        }
    }));
    
    if (ctx.feedback_message) {
        rows.push_back(ftxui::Renderer([ctx] {
            if (ctx.feedback_message->empty()) return ftxui::text("");
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? ftxui::Color::Green : ftxui::Color::Red;
            return ftxui::text(*ctx.feedback_message) | ftxui::bold | ftxui::color(color);
        }));
    }
    
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::flex | ftxui::frame;
}

static ftxui::Component CreateFindForm(TableBase* table, ModalFactory::Context ctx) {
    return ftxui::Renderer([table, ctx] {
        auto titles = table->GetTitles();
        if (titles.empty()) return ftxui::text("Ошибка: нет заголовков");
        
        std::vector<ftxui::Component> rows;
        
        rows.push_back(ftxui::Container::Horizontal({
            ftxui::Renderer([title = titles[0]] { return ftxui::text(title + ": "); }),
            ftxui::Input(ctx.search_key, "Введите значение...") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)
        }));
        
        rows.push_back(ftxui::Button("Найти", [table, ctx] {
            if (!ctx.search_key || !ctx.result_index || !ctx.result_record) return;
            
            *ctx.result_index = table->Find(*ctx.search_key);
            if (*ctx.result_index >= 0) {
                *ctx.result_record = table->GetDataAt(*ctx.result_index);
            }
        }));
        
        rows.push_back(ftxui::Renderer([ctx] {
            if (!ctx.result_index || !ctx.result_record) return ftxui::text("");
            
            if (*ctx.result_index == -1 || ctx.result_record->isEmpty) {
                return ftxui::text("🔍 Нет данных") | ftxui::dim | ftxui::center;
            }
            
            std::vector<ftxui::Element> elements;
            for (size_t i = 0; i < ctx.result_record->fields.size(); ++i) {
                if (i == 0) {
                    elements.push_back(ftxui::text(ctx.result_record->fields[i]) | ftxui::bold | ftxui::color(ftxui::Color::Cyan));
                } else {
                    elements.push_back(ftxui::text(ctx.result_record->fields[i]));
                }
                if (i < ctx.result_record->fields.size() - 1) {
                    elements.push_back(ftxui::text(" | ") | ftxui::dim);
                }
            }
            return hbox(elements) | ftxui::borderLight;
        }));
        
        return vbox(std::move(rows)) | ftxui::flex;
    });
}

static ftxui::Component CreateCollisionsView(TableBase* table) {
    return ftxui::Renderer([table] {
        auto hash_table = dynamic_cast<HashTable*>(table);
        if (hash_table) {
            int count = hash_table->Collisions();
            return ftxui::hbox({
                ftxui::text("Коллизии: "),
                ftxui::text(std::to_string(count)) | ftxui::bold | ftxui::color(ftxui::Color::Magenta)
            }) | ftxui::center | ftxui::flex;
        }
        return ftxui::text("Н/Д") | ftxui::dim | ftxui::center;
    });
}

ftxui::Component ModalFactory::create(TableType type, ModalAction action, TableBase* table, Context ctx) {
    ftxui::Color header_color = (type == TableType::Regular) ? ftxui::Color::Cyan : ftxui::Color::Green;
    
    switch (action) 
    {
        // case ModalAction::Print:
        //     return CreatePrintView(table, header_color);
            
        // case ModalAction::Add:
        //     return CreateAddForm(table, ctx);
            
        // case ModalAction::Edit:
        //     return CreateEditForm(table, ctx);
            
        // case ModalAction::Delete:
        //     return CreateDeleteForm(table, ctx);
            
        // case ModalAction::Find:
        //     return CreateFindForm(table, ctx);
            
        // case ModalAction::Collisions:
        //     return (type == TableType::Hash) 
        //         ? CreateCollisionsView(table) 
        //         : ftxui::Renderer([] { 
        //             return ftxui::text("Только для HashTable") | ftxui::dim;
        //         });
            
        default:
            return ftxui::Renderer([] { 
                return ftxui::text("Неизвестное действие") | ftxui::dim; 
            });
    }
}