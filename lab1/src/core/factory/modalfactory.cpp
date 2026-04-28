#include <modalfactory.hpp>

#include <ftxui/dom/table.hpp>
#include <algorithm>
#include <itable.hpp>
#include <hashtable.hpp>
#include <regulartable.hpp>
#include <sstream>

ftxui::Component ModalFactory::createFeedbackRenderer(Context ctx) const {
    return ftxui::Renderer([ctx] {
        if (!ctx.feedback_message || ctx.feedback_message->empty())
            return ftxui::text("");
        bool success = ctx.operation_success && *ctx.operation_success;
        auto color = success ? ftxui::Color::Green : ftxui::Color::Red;
        return ftxui::text(*ctx.feedback_message) | ftxui::bold | ftxui::color(color);
    });
}
std::vector<ftxui::Component> ModalFactory::createInputRows(const std::vector<std::string>& titles, Context ctx, int width) const {
    std::vector<ftxui::Component> rows;
    if (!ctx.input_fields) return rows;

    if (ctx.input_fields->empty()) {
        ctx.input_fields->resize(titles.size());
        std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }

    size_t limit = std::min(titles.size(), ctx.input_fields->size());
    for (size_t i = 0; i < limit; ++i) {
        rows.push_back(ftxui::Container::Horizontal({
            ftxui::Renderer([title = titles[i]] { return ftxui::text(title + ": "); }),
            ftxui::Input(&(*ctx.input_fields)[i], "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, width)
        }));
    }
    return rows;
}
ftxui::Component ModalFactory::createKeyInputRow(const std::string& label, Context ctx) const {
    return ftxui::Container::Horizontal({
        ftxui::Renderer([label] { return ftxui::text(label + ": "); }),
        ftxui::Input(ctx.search_key, "") | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30)
    });
}

ftxui::Component ModalFactory::create(TableType type, ModalAction action, ITable* table, Context ctx) 
{
    ftxui::Color header_color = (type == TableType::Regular) ? ftxui::Color::Cyan : ftxui::Color::Green;
    auto hashOnlyFallback = ftxui::Renderer([] { 
        return ftxui::text("Только для HashTable") | ftxui::dim;
    });
    switch (action) {
        case ModalAction::Print:       return m_createPrintView(table, header_color);
        case ModalAction::Add:         return m_createAddForm(table, ctx);
        case ModalAction::Edit:        return m_createEditForm(table, ctx);
        case ModalAction::Delete:      return m_createDeleteForm(table, ctx);
        case ModalAction::Find:        return m_createFindForm(table, ctx);
        case ModalAction::Collisions:  return (type == TableType::Hash) ? m_createCollisionsView(table) : hashOnlyFallback;
        case ModalAction::Graph:       return (type == TableType::Hash) ? m_createGraphView(table) : hashOnlyFallback;
        default:                       return ftxui::Renderer([] { return ftxui::text("Неизвестное действие") | ftxui::dim; });
    }
}

ftxui::Component ModalFactory::m_createPrintView(ITable* table, ftxui::Color header_color) 
{
    return ftxui::Renderer([table, header_color] {
        auto data = table->getData();
        auto titles = table->getTitles();
        
        std::vector<std::vector<std::string>> entries;
        entries.reserve(data.size());
        for (const auto& rec : data) entries.push_back(rec.fields);

        const int rows_per_table = 16;
        std::vector<ftxui::Element> tables;
        tables.reserve(4);

        for (size_t chunk = 0; chunk < 4; ++chunk) {
            std::vector<std::vector<std::string>> chunk_entries;
            chunk_entries.push_back(titles);
            
            int start = chunk * rows_per_table;
            int end = std::min(start + rows_per_table, static_cast<int>(entries.size()));
            for (size_t i = start; i < end; ++i) chunk_entries.push_back(entries[i]);

            auto t = ftxui::Table(chunk_entries);
            t.SelectAll().Border(ftxui::LIGHT);
            t.SelectAll().SeparatorHorizontal(ftxui::LIGHT);
            t.SelectAll().SeparatorVertical(ftxui::LIGHT);
            t.SelectRow(0).Decorate(ftxui::bold | ftxui::color(header_color));
            
            tables.push_back(t.Render());
        }

        return ftxui::hbox(tables) | ftxui::frame | ftxui::vscroll_indicator;
    });
}

ftxui::Component ModalFactory::m_createAddForm(ITable* table, Context ctx) 
{
    auto titles = table->getTitles();
    std::vector<ftxui::Component> rows = createInputRows(titles, ctx);

    rows.push_back(ftxui::Button("Добавить", [table, ctx] {
        if (!ctx.input_fields) return;
        bool all_filled = std::all_of(ctx.input_fields->begin(), ctx.input_fields->end(),
            [](const std::string& s) { return !s.empty(); });
            
        if (!all_filled) {
            if (ctx.feedback_message) *ctx.feedback_message = "Заполните поля до конца";
            return;
        }

        Record rec{*ctx.input_fields};
        int index = table->indexOfRecord((*ctx.input_fields)[0], RecordMethod::FREE_RECORD);
        bool success = table->add(rec, index);
        
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) *ctx.feedback_message = success ? "Добавлено" : "Ошибка";
        if (success) std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }));

    rows.push_back(createFeedbackRenderer(ctx));
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createEditForm(ITable* table, Context ctx) 
{
    auto titles = table->getTitles();
    std::vector<ftxui::Component> rows = createInputRows(titles, ctx);

    rows.push_back(ftxui::Button("Редактировать запись", [table, ctx] {
        if (!ctx.input_fields) return;
        bool all_filled = std::all_of(ctx.input_fields->begin(), ctx.input_fields->end(),
            [](const std::string& s) { return !s.empty(); });
            
        if (!all_filled) {
            if (ctx.feedback_message) *ctx.feedback_message = "Заполните поля до конца";
            return;
        }

        Record rec{*ctx.input_fields};
        int index = table->indexOfRecord((*ctx.input_fields)[0], RecordMethod::INDEX_RECORD);
        bool success = table->modify(rec, index);
        
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) *ctx.feedback_message = success ? "Отредактировано" : "Ошибка";
        if (success) std::fill(ctx.input_fields->begin(), ctx.input_fields->end(), "");
    }));

    rows.push_back(createFeedbackRenderer(ctx));
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createDeleteForm(ITable* table, Context ctx) 
{
    auto key_title = table->getTitles().empty() ? "Ключ" : table->getTitles()[0];
    std::vector<ftxui::Component> rows;
    rows.push_back(createKeyInputRow(key_title, ctx));

    rows.push_back(ftxui::Button("Удалить", [table, ctx] {
        if (!ctx.search_key || ctx.search_key->empty()) {
            if (ctx.feedback_message) *ctx.feedback_message = "Введите что-нибудь";
            return;
        }

        int index = table->indexOfRecord(*ctx.search_key, RecordMethod::INDEX_RECORD);
        bool success = table->remove(*ctx.search_key, index);
        
        if (ctx.operation_success) *ctx.operation_success = success;
        if (ctx.feedback_message) *ctx.feedback_message = success ? "Удалено" : "Не найдено";
        if (success) *ctx.search_key = "";
    }));

    rows.push_back(createFeedbackRenderer(ctx));
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createFindForm(ITable* table, Context ctx) 
{
    auto key_title = table->getTitles().empty() ? "Ключ" : table->getTitles()[0];
    std::vector<ftxui::Component> rows;
    rows.push_back(createKeyInputRow(key_title, ctx));

    rows.push_back(ftxui::Button("Найти запись", [table, ctx] {
        if (!ctx.search_key || ctx.search_key->empty()) {
            if (ctx.feedback_message) *ctx.feedback_message = "Введите что-нибудь";
            return;
        }

        int index = table->indexOfRecord(*ctx.search_key, RecordMethod::INDEX_RECORD);
        auto result = table->find(*ctx.search_key, index);
        
        if (result.result) {
            if (ctx.feedback_message) {
                std::stringstream ss;
                ss << "Найдено: " << result.record.fields[1] << " " << result.record.fields[2];
                *ctx.feedback_message = ss.str();
            }
            *ctx.search_key = "";
        } else {
            if (ctx.feedback_message) *ctx.feedback_message = "Не найдено";
        }
        if (ctx.operation_success) *ctx.operation_success = result.result;
    }));

    rows.push_back(createFeedbackRenderer(ctx));
    return ftxui::Container::Vertical(std::move(rows)) | ftxui::frame;
}

ftxui::Component ModalFactory::m_createCollisionsView(ITable* table) 
{
    return ftxui::Renderer([table] {
        auto* ht = dynamic_cast<HashTable*>(table);
        if (!ht) return ftxui::text("Н/Д") | ftxui::dim | ftxui::center;

        auto collisions = ht->showStatistics();
        auto count = ht->getData().size();
        return ftxui::hbox({
            ftxui::text("Коллизии: "),
            ftxui::text(std::to_string(collisions)) | ftxui::bold | ftxui::color(ftxui::Color::Magenta),
            ftxui::separator(),
            ftxui::text("Кол-во записей: "),
            ftxui::text(std::to_string(count)) | ftxui::bold | ftxui::color(ftxui::Color::Magenta)
        }) | ftxui::center | ftxui::flex;
    });
}

ftxui::Component ModalFactory::m_createGraphView(ITable* table) 
{
    return ftxui::Renderer([table] {
        auto* ht = dynamic_cast<HashTable*>(table);
        if (!ht) return ftxui::text("Н/Д") | ftxui::dim | ftxui::center;

        auto collisions = ht->showStatistics();
        auto count = ht->getData().size();
        if (count == 0) return ftxui::text("Нет данных") | ftxui::dim | ftxui::center;

        ftxui::Canvas c(count, collisions);
        float slope = static_cast<float>(collisions) / count;
        
        for (size_t x = 0; x < count - 1; ++x) {
            int y1 = collisions - 1 - static_cast<int>(slope * x);
            int y2 = collisions - 1 - static_cast<int>(slope * (x + 1));
            c.DrawPointLine(x, y1, x + 1, y2);
        }
        return ftxui::canvas(std::move(c)) | ftxui::center;
    });
}