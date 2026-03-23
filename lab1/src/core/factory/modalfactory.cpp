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

    std::vector<ftxui::Component> rows;
    
    for (size_t i = 0; i < titles.size() && ctx.input_fields && i < ctx.input_fields->size(); ++i) {
        rows.push_back(ftxui::Container::Horizontal({
            ftxui::Renderer([title = titles[i]] { return ftxui::text(title + ": "); }),
            ftxui::Input(&(*ctx.input_fields)[i], "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 25)
        }));
    }

    rows.push_back(ftxui::Button("Редактировать запись", [table, ctx] {
        if (!ctx.input_fields) return;
        bool all_filled = std::all_of(ctx.input_fields->begin(), ctx.input_fields->end(),
            [](const std::string& s) { return !s.empty(); });
        
        if (all_filled) {
            Record rec;
            rec.fields = *ctx.input_fields;
            rec.isEmpty = false;
            bool success = table->Modify(rec.fields);
            if (ctx.operation_success) *ctx.operation_success = success;
            if (ctx.feedback_message) {
                *ctx.feedback_message = success ? "✓ Отредактировано" : "✗ Ошибка";
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
    auto key_title = table->GetTitles().empty() ? "Ключ" : table->GetTitles()[0];
    std::vector<ftxui::Component> rows;
    
    rows.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([key_title] { return ftxui::text(key_title + ": "); }),
        ftxui::Input(ctx.search_key, "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)
    }));
    
    rows.push_back(ftxui::Button("Найти запись", [table, ctx] {
        auto data = table->GetData();
        if (!ctx.search_key || ctx.search_key->empty()) return;
        int index = table->Find(*ctx.search_key);
        auto success = index != -1;
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) {
            std::stringstream ss;
            if(success)
                ss << data[index].fields[1] << data[index].fields[2];
            else  
                ss << "Не найдено";
            *ctx.feedback_message = ss.str();
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
            return (type == TableType::Hash) 
                ? CreateCollisionsView(table) 
                : ftxui::Renderer([] { 
                    return ftxui::text("Только для HashTable") | ftxui::dim;
                });
            
        default:
            return ftxui::Renderer([] { 
                return ftxui::text("Неизвестное действие") | ftxui::dim; 
            });
    }
}