#include <modalfactory.hpp>

#include <ftxui/dom/table.hpp>
#include <algorithm>

#include <tablebase.hpp>
#include <hashtable.hpp>

ftxui::Component ModalFactory::create(TableType type, ModalAction action, TableBase* table, Context ctx) {
    ftxui::Color header_color = (type == TableType::Regular) ? ftxui::Color::Cyan : ftxui::Color::Green;
    
    switch (action) 
    {
        case ModalAction::Print:
            return m_createPrintView(table, header_color);
            
        case ModalAction::Add:
            return m_createAddForm(table, ctx);
            
        case ModalAction::Edit:
            return m_createEditForm(table, ctx);
            
        case ModalAction::Delete:
            return m_createDeleteForm(table, ctx);
            
        case ModalAction::Find:
            return m_createFindForm(table, ctx);
            
        case ModalAction::Collisions:
            return (type == TableType::Hash) 
                ? m_createCollisionsView(table) 
                : ftxui::Renderer([] { 
                    return ftxui::text("Только для HashTable") | ftxui::dim;
                });
            
        default:
            return ftxui::Renderer([] { 
                return ftxui::text("Неизвестное действие") | ftxui::dim; 
            });
    }
}

ftxui::Component ModalFactory::m_createPrintView(TableBase* table, ftxui::Color header_color) {
    auto renderer = ftxui::Renderer([table, header_color] {
        auto data = table->getData();
        auto titles = table->getTitles();
        
        std::vector<std::vector<std::string>> entries;
        entries.push_back(titles);
        for (const auto& rec : data) {
            if (!rec.isEmpty) entries.push_back(rec.fields);
        }

        const int rows_per_table = 16;
        std::vector<ftxui::Element> tables;

        for (int chunk = 0; chunk < 4; ++chunk) {
            std::vector<std::vector<std::string>> chunk_entries;
            chunk_entries.push_back(titles);
            
            int start_row = 1 + chunk * rows_per_table;
            int end_row = std::min(start_row + rows_per_table, static_cast<int>(entries.size()));
            
            for (int i = start_row; i < end_row; ++i) {
                chunk_entries.push_back(entries[i]);
            }

            auto t = ftxui::Table(chunk_entries);
            t.SelectAll().Border(ftxui::LIGHT);
            t.SelectAll().SeparatorHorizontal(ftxui::LIGHT);
            t.SelectAll().SeparatorVertical(ftxui::LIGHT);
            t.SelectRow(0).Decorate(ftxui::bold | ftxui::color(header_color));
            
            tables.push_back(t.Render());
        }

        auto content = ftxui::hbox(tables) | ftxui::frame | ftxui::vscroll_indicator;
        return content;
    });
    
    return ftxui::Container::Vertical({renderer});
}

ftxui::Component ModalFactory::m_createAddForm(TableBase* table, ModalFactory::Context ctx) {
    auto titles = table->getTitles();
    
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
            bool success = table->add(rec);
            if (ctx.operation_success) *ctx.operation_success = success;
            if (ctx.feedback_message) {
                *ctx.feedback_message = success ? "Добавлено" : "Ошибка";
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

    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createEditForm(TableBase* table, ModalFactory::Context ctx) {
    auto titles = table->getTitles();
    
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
            bool success = table->modify(rec.fields);
            if (ctx.operation_success) *ctx.operation_success = success;
            if (ctx.feedback_message) {
                *ctx.feedback_message = success ? "Отредактировано" : "Ошибка";
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

    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createDeleteForm(TableBase* table, ModalFactory::Context ctx) {
    auto key_title = table->getTitles().empty() ? "Ключ" : table->getTitles()[0];
    
    std::vector<ftxui::Component> rows;
    
    rows.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([key_title] { return ftxui::text(key_title + ": "); }),
        ftxui::Input(ctx.search_key, "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)
    }));
    
    rows.push_back(ftxui::Button("Удалить", [table, ctx] {
        if (!ctx.search_key || ctx.search_key->empty()) return;
        bool success = table->remove(*ctx.search_key);
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) {
            *ctx.feedback_message = success ? "Удалено" : "Не найдено";
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
    
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createFindForm(TableBase* table, ModalFactory::Context ctx) {
    auto key_title = table->getTitles().empty() ? "Ключ" : table->getTitles()[0];
    std::vector<ftxui::Component> rows;
    
    rows.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([key_title] { return ftxui::text(key_title + ": "); }),
        ftxui::Input(ctx.search_key, "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)
    }));
    
    rows.push_back(ftxui::Button("Найти запись", [table, ctx] {
        if (!ctx.search_key || ctx.search_key->empty()) return;
        int index = table->find(*ctx.search_key);
        auto data = table->getDataAt(index);
        auto success = index != -1;
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) {
            std::stringstream ss;
            if(success)
                ss << data.fields[1] << data.fields[2];
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
    
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createCollisionsView(TableBase* table) {
    return ftxui::Renderer([table] {
        auto hash_table = dynamic_cast<HashTable*>(table);
        if (hash_table) {
            int count = hash_table->getTotalCollisions();
            return ftxui::hbox({
                ftxui::text("Коллизии: "),
                ftxui::text(std::to_string(count)) | ftxui::bold | ftxui::color(ftxui::Color::Magenta)
            }) | ftxui::center | ftxui::flex;
        }
        return ftxui::text("Н/Д") | ftxui::dim | ftxui::center;
    });
}