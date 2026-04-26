#include <modalfactory.hpp>

#include <ftxui/dom/table.hpp>
#include <algorithm>

#include <itable.hpp>
#include <hashtable.hpp>
#include <regulartable.hpp>
#include <fstream>

int getIndexOfKey(ITable* table, const std::string& key)
{
    int index = -1;
    if(table == nullptr)
        return index;

    if (auto ht = dynamic_cast<HashTable*>(table))
        index = ht->hashFunction(key);
    else if(auto rt = dynamic_cast<RegularTable*>(table))
        index = rt->indexOfFreeRecord();

    return index;
}

ftxui::Component ModalFactory::create(TableType type, ModalAction action, ITable* table, Context ctx) 
{
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
        case ModalAction::Graph:
            return (type == TableType::Hash) 
                ? m_createGraphView(table) 
                : ftxui::Renderer([] { 
                    return ftxui::text("Только для HashTable") | ftxui::dim;
                });
            
        default:
            return ftxui::Renderer([] { 
                return ftxui::text("Неизвестное действие") | ftxui::dim; 
            });
    }
}

ftxui::Component ModalFactory::m_createPrintView(ITable* table, ftxui::Color header_color) 
{
    auto renderer = ftxui::Renderer([table, header_color] {
        auto data = table->getData();
        auto titles = table->getTitles();
        
        std::vector<std::vector<std::string>> entries;
        
        for (const auto& rec : data) 
        {
            entries.push_back(rec.fields);
        }

        const int rows_per_table = 16;
        std::vector<ftxui::Element> tables;
        for (int chunk = 0; chunk < 4; chunk++) 
        {
            std::vector<std::vector<std::string>> chunk_entries;
            chunk_entries.push_back(titles);
            
            int start_row = chunk * rows_per_table;
            int end_row = std::min(start_row + rows_per_table, static_cast<int>(entries.size()));
            for (int i = start_row; i < end_row; i++) chunk_entries.push_back(entries[i]);

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

ftxui::Component ModalFactory::m_createAddForm(ITable* table, ModalFactory::Context ctx) 
{
    auto titles = table->getTitles();
    if (ctx.input_fields && ctx.input_fields->empty()) 
    {
        ctx.input_fields->resize(titles.size());
        std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }

    std::vector<ftxui::Component> rows;
    for (size_t i = 0; i < titles.size() && ctx.input_fields && i < ctx.input_fields->size(); i++) 
    {
        rows.push_back(ftxui::Container::Horizontal({
            ftxui::Renderer([title = titles[i]] { return ftxui::text(title + ": "); }),
            ftxui::Input(&(*ctx.input_fields)[i], "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 25)
        }));
    }
    rows.push_back(ftxui::Button("Добавить", [table, ctx] {
        if (!ctx.input_fields) 
            return;
        bool all_filled = std::all_of(ctx.input_fields->begin(), ctx.input_fields->end(),
            [](const std::string& s) { return !s.empty(); });
        
        if (!all_filled) return;
        Record rec;
        rec.fields = *ctx.input_fields;

        auto key = (*ctx.input_fields)[0];
        int index = getIndexOfKey(table, key);
        auto success = table->add(rec, index);
        if (ctx.operation_success) 
            *ctx.operation_success = success;
        if (ctx.feedback_message) 
            *ctx.feedback_message = success ? "Добавлено" : "Ошибка";
        
        if (success)
            std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
        
    }));

    if (ctx.feedback_message) 
    {
        rows.push_back(ftxui::Renderer([ctx] {
            if (ctx.feedback_message->empty()) return ftxui::text("");
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? ftxui::Color::Green : ftxui::Color::Red;
            return ftxui::text(*ctx.feedback_message) | ftxui::bold | ftxui::color(color);
        }));
    }

    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createEditForm(ITable* table, ModalFactory::Context ctx) 
{
    auto titles = table->getTitles();
    if (ctx.input_fields && ctx.input_fields->empty()) 
    {
        ctx.input_fields->resize(titles.size());
        std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }

    std::vector<ftxui::Component> rows;   
    for (size_t i = 0; i < titles.size() && ctx.input_fields && i < ctx.input_fields->size(); i++) 
    {
        rows.push_back(ftxui::Container::Horizontal({
            ftxui::Renderer([title = titles[i]] { return ftxui::text(title + ": "); }),
            ftxui::Input(&(*ctx.input_fields)[i], "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 25)
        }));
    }
    rows.push_back(ftxui::Button("Редактировать запись", [table, ctx] {
        if (!ctx.input_fields) return;
        bool all_filled = std::all_of(ctx.input_fields->begin(), ctx.input_fields->end(),
            [](const std::string& s) { return !s.empty(); });
        
        if (!all_filled) return;
        Record rec;
        rec.fields = *ctx.input_fields;

        auto key = (*ctx.input_fields)[0];
        int index = table->indexOfRecord(key);
        bool success = table->modify(rec, index);
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) *ctx.feedback_message = success ? "Отредактировано" : "Ошибка";

        if (success)
            std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }));

    if (ctx.feedback_message)
    {
        rows.push_back(ftxui::Renderer([ctx] {
            if (ctx.feedback_message->empty()) return ftxui::text("");
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? ftxui::Color::Green : ftxui::Color::Red;
            return ftxui::text(*ctx.feedback_message) | ftxui::bold | ftxui::color(color);
        }));
    }
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createDeleteForm(ITable* table, ModalFactory::Context ctx) 
{
    auto key_title = table->getTitles().empty() ? "Ключ" : table->getTitles()[0];
    
    std::vector<ftxui::Component> rows;
    rows.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([key_title] { return ftxui::text(key_title + ": "); }),
        ftxui::Input(ctx.search_key, "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)
    }));
    rows.push_back(ftxui::Button("Удалить", [table, ctx] {
        if (!ctx.search_key || ctx.search_key->empty()) return;

        auto key = *ctx.search_key;
        int index = table->indexOfRecord(key);
        bool success = table->remove(key, index);
        if (ctx.operation_success) 
            *ctx.operation_success = success;
        if (ctx.feedback_message) 
            *ctx.feedback_message = success ? "Удалено" : "Не найдено";
            
        if (success) 
            std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
        
    }));
    
    if (ctx.feedback_message) 
    {
        rows.push_back(ftxui::Renderer([ctx] {
            if (ctx.feedback_message->empty()) return ftxui::text("");
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? ftxui::Color::Green : ftxui::Color::Red;
            return ftxui::text(*ctx.feedback_message) | ftxui::bold | ftxui::color(color);
        }));
    }
    
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createFindForm(ITable* table, ModalFactory::Context ctx) 
{
    auto key_title = table->getTitles().empty() ? "Ключ" : table->getTitles()[0];

    std::vector<ftxui::Component> rows;
    rows.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([key_title] { return ftxui::text(key_title + ": "); }),
        ftxui::Input(ctx.search_key, "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)
    }));
    rows.push_back(ftxui::Button("Найти запись", [table, ctx] {
        if (!ctx.search_key || ctx.search_key->empty()) 
            return;

        auto key = *ctx.search_key;
        int index = table->indexOfRecord(key);
        auto success = table->find(key, index);
        if(success)
        {     
            auto data = table->getData()[index];
            if (ctx.operation_success)
                *ctx.operation_success = success;
    
            if (ctx.feedback_message) 
            {
                std::stringstream ss;
                if(success)
                    ss << "Найдено: " << data.fields[1] << " " << data.fields[2];
                else  
                    ss << "Не найдено";
                *ctx.feedback_message = ss.str();
            }
    
            std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
        }
    }));
    
    if (ctx.feedback_message) 
    {
        rows.push_back(ftxui::Renderer([ctx] {
            if (ctx.feedback_message->empty()) return ftxui::text("");
            bool success = (ctx.operation_success && *ctx.operation_success);
            auto color = success ? ftxui::Color::Green : ftxui::Color::Red;
            return ftxui::text(*ctx.feedback_message) | ftxui::bold | ftxui::color(color);
        }));
    }
    
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createCollisionsView(ITable* table) 
{
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

ftxui::Component ModalFactory::m_createGraphView(ITable* table) 
{
    return ftxui::Renderer([table] {
        auto hash_table = dynamic_cast<HashTable*>(table);
        if (hash_table) 
        {
            int collisions = 10;
            int count = hash_table->getData().size();
            auto graph = ftxui::graph([&](int width, int height) {
                std::vector<int> output;
                output.push_back(collisions);
                output.push_back(count);
                return output;
            });
            return graph | ftxui::size(ftxui::WIDTH, ftxui::Constraint::EQUAL, 200) | ftxui::size(ftxui::HEIGHT, ftxui::Constraint::EQUAL, 200);
        }
        return ftxui::text("Н/Д") | ftxui::dim | ftxui::center;
    });
}