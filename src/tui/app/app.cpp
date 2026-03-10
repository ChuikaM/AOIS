#include <app.hpp>

#include <menu.hpp>
#include <settabcommand.hpp>
#include <actioncommand.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/table.hpp>

#include <hashtable.hpp>
#include <txttable.hpp>
#include <modalfactory.hpp>

App::App()
{
    txtTable = std::make_unique<TxtTable>();
    hashTable = std::make_unique<HashTable>();
    m_record = std::make_unique<Record>();
}
App::~App() = default;

void App::Run()
{
    auto screen = ftxui::ScreenInteractive::TerminalOutput();
    
    std::vector<std::pair<std::string, std::unique_ptr<ICommand>>> mainMenuOptions;
    mainMenuOptions.emplace_back("Работа с таблицей", std::make_unique<SetTabCommand>(m_current_tab, 1));
    mainMenuOptions.emplace_back("Работа с хэш-таблицей", std::make_unique<SetTabCommand>(m_current_tab, 2));
    mainMenuOptions.emplace_back("Выход", std::make_unique<ActionCommand>([&screen]{screen.ExitLoopClosure()();}));
    Menu mainMenu(std::move(mainMenuOptions));
    auto menu1 = mainMenu.CreateMenu();

    std::vector<std::pair<std::string, std::unique_ptr<ICommand>>> tableMenuOptions;
    tableMenuOptions.emplace_back("Назад", std::make_unique<SetTabCommand>(m_current_tab, 0));
    tableMenuOptions.emplace_back("Вывести таблицу", std::make_unique<SetTabCommand>(m_current_tab, 3));
    tableMenuOptions.emplace_back("Добавление", std::make_unique<SetTabCommand>(m_current_tab, 4));
    tableMenuOptions.emplace_back("Редактирование", std::make_unique<SetTabCommand>(m_current_tab, 5));
    tableMenuOptions.emplace_back("Удаление", std::make_unique<SetTabCommand>(m_current_tab, 6));
    tableMenuOptions.emplace_back("Поиск", std::make_unique<SetTabCommand>(m_current_tab, 7));
    Menu tableMenu(std::move(tableMenuOptions));
    auto menu2 = tableMenu.CreateMenu();

    std::vector<std::pair<std::string, std::unique_ptr<ICommand>>> hashTableMenuOptions;
    hashTableMenuOptions.emplace_back("Назад", std::make_unique<SetTabCommand>(m_current_tab, 0));
    hashTableMenuOptions.emplace_back("Вывести таблицу", std::make_unique<SetTabCommand>(m_current_tab, 8));
    hashTableMenuOptions.emplace_back("Добавление", std::make_unique<SetTabCommand>(m_current_tab, 9));
    hashTableMenuOptions.emplace_back("Редактирование", std::make_unique<SetTabCommand>(m_current_tab, 10));
    hashTableMenuOptions.emplace_back("Удаление", std::make_unique<SetTabCommand>(m_current_tab, 11));
    hashTableMenuOptions.emplace_back("Поиск", std::make_unique<SetTabCommand>(m_current_tab, 12));
    hashTableMenuOptions.emplace_back("Коллизии", std::make_unique<SetTabCommand>(m_current_tab, 13));
    Menu hashTableMenu(std::move(hashTableMenuOptions));
    auto menu3 = hashTableMenu.CreateMenu();

    auto tabs = ftxui::Container::Tab({
        menu1, menu2, menu3, 
        ModalSimpleTablePrint(), ModalSimpleTableAdd(), ModalSimpleTableEdit(), ModalSimpleTableDelete(), ModalSimpleTableFind(),
        ModalHashTablePrint(), ModalHashTableAdd(), ModalHashTableEdit(), ModalHashTableDelete(), ModalHashTableFind(), ModalHashTableCollisions()
    }, &m_current_tab);

    std::vector<std::string> titles {
        "Главное меню", "Работа с таблицей", "Работа с хэш-таблицей", 
        "Вывод таблицы", "Добавление в таблицу", "Редактирование таблицы", "Удаление в таблице", "Поиск в таблице",
        "Вывод хэш-таблицы", "Добавление в хэш-таблицу", "Редактирование хэш-таблицы", "Удаление в хэш-таблице", "Поиск в хэш-таблице", "Коллизии"
    };
    auto renderer = ftxui::Renderer(tabs, [&] {
        return ftxui::vbox({
            ftxui::text(titles[m_current_tab]) | ftxui::bold | ftxui::center,
            ftxui::separator(),
            tabs->Render() | ftxui::flex | ftxui::frame | ftxui::vscroll_indicator,
            ftxui::separator(),
            ftxui::text(m_current_tab == 0 ? "Enter: Выбор" : "Enter: Выбор | Esc: Назад") | ftxui::dim | ftxui::center,
        }) | ftxui::border | ftxui::flex | ftxui::frame | ftxui::vscroll_indicator;
    });

    renderer |= ftxui::CatchEvent([&](ftxui::Event e) {
        if (e == ftxui::Event::Escape) {
            if(m_current_tab == 1 || m_current_tab == 2)
            {
                m_current_tab = 0;
                return true;
            }
            else if(m_current_tab > 2 && m_current_tab < 8)
            {
                m_current_tab = 1;
                return true;
            }
            else if(m_current_tab >= 8)
            {
                m_current_tab = 2;
                return true;
            }
        }
        if (e == ftxui::Event::CtrlC) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(renderer);
}

ftxui::Component App::ModalSimpleTablePrint()
{
    using namespace ftxui;
    return Renderer([this] {
        auto data = txtTable->GetData();
        auto titles = txtTable->GetTitles();
        
        std::vector<std::vector<std::string>> entries;
        entries.push_back(titles);
    
        for (const auto& record : data) {
            if (record.isEmpty) continue;
            
            entries.push_back(record.fields);
        }

        auto table = Table(entries);
        table.SelectAll().Border(LIGHT);
        table.SelectAll().SeparatorHorizontal(LIGHT);
        table.SelectAll().SeparatorVertical(LIGHT);
        table.SelectRow(0).Decorate(bold | color(Color::Cyan));

        return table.Render() | flex | frame | vscroll_indicator;
    });
}
ftxui::Component App::ModalSimpleTableAdd()
{
    auto titles = txtTable->GetTitles();
    m_titles_content.resize(titles.size());
    
    std::vector<ftxui::Component> inputs;
    for(size_t i = 0; i < titles.size(); i++)
    {
        inputs.push_back(
            ftxui::Container::Horizontal({
                ftxui::Renderer([title = titles[i]] { 
                    return ftxui::text(title + ": "); 
                }),
                ftxui::Input(&m_titles_content[i], titles[i])
            })
        );
    }
    inputs.push_back(ftxui::Button("Добавить", [this]{
        if(m_titles_content[0].empty() || m_titles_content[1].empty() || m_titles_content[2].empty()) 
            return;
        
        m_row_added = txtTable->Add({ m_titles_content });
    }));
    inputs.push_back(ftxui::Renderer([this]{
        return ftxui::text(m_row_added ? "Данные успешно добавлены." : "Не удалось добавить данные в таблицу!");
    }));

    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalSimpleTableEdit()
{
    auto titles = txtTable->GetTitles();
    m_titles_content.resize(titles.size());
    
    std::vector<ftxui::Component> inputs;
    for(size_t i = 0; i < titles.size(); i++)
    {
        inputs.push_back(
            ftxui::Container::Horizontal({
                ftxui::Renderer([title = titles[i]] { 
                    return ftxui::text(title + ": "); 
                }),
                ftxui::Input(&m_titles_content[i], titles[i])
            })
        );
    }
    inputs.push_back(ftxui::Button("Изменить", [this]{
        m_modified = txtTable->Modify( m_titles_content );
    }));
    inputs.push_back(ftxui::Renderer([this]{
        return ftxui::text(m_modified ? "Данные успешно Изменены." : "Данные не внесены");
    }));
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalSimpleTableDelete()
{
    auto key_title = txtTable->GetTitles()[0];
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([key_title]{
            return ftxui::text(key_title) | ftxui::vcenter; 
        }),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    inputs.push_back(ftxui::Button("Удалить", [this]{
        m_deleted = txtTable->Delete(m_content);
    }));
    inputs.push_back(ftxui::Renderer([this]{
        return ftxui::text(m_deleted ? "Данные успешно Удалены." : "Данные не удалены");
    }));
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalSimpleTableFind()
{
    auto titles = txtTable->GetTitles();
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([title = titles[0]]{
            return ftxui::text(title) | ftxui::vcenter; 
        }),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    
    inputs.push_back(ftxui::Button("Найти", [this]{
        m_index = txtTable->Find(m_content);
        if (m_index != -1) {
            *m_record = txtTable->GetDataAt(m_index);
        }
    }));
    
    inputs.push_back(ftxui::Renderer([this]{
        if (m_index == -1 || m_record->isEmpty) {
            return ftxui::text("Нет данных") | ftxui::dim;
        }
        
        std::vector<ftxui::Element> elements;
        for (size_t i = 0; i < m_record->fields.size(); ++i) {
            elements.push_back(ftxui::text(m_record->fields[i]));
            if (i < m_record->fields.size() - 1) {
                elements.push_back(ftxui::text(" | "));
            }
        }
        return ftxui::hbox(elements);
    }));
    
    return ftxui::Container::Vertical(std::move(inputs));
}


ftxui::Component App::ModalHashTablePrint()
{
    using namespace ftxui;
    return Renderer([this] {
        auto data = hashTable->GetData();
        auto titles = hashTable->GetTitles();
        
        std::vector<std::vector<std::string>> entries;
        entries.push_back(titles);
    
        for (const auto& record : data) {
            if (record.isEmpty) continue;

            entries.push_back(record.fields);
        }

        auto table = Table(entries);
        table.SelectAll().Border(LIGHT);
        table.SelectAll().SeparatorHorizontal(LIGHT);
        table.SelectAll().SeparatorVertical(LIGHT);
        table.SelectRow(0).Decorate(bold | color(Color::Cyan));

        return table.Render() | flex | frame | vscroll_indicator;
    });
}
ftxui::Component App::ModalHashTableAdd()
{
   auto titles = hashTable->GetTitles();
    m_titles_content.resize(titles.size());
    
    std::vector<ftxui::Component> inputs;
    for(size_t i = 0; i < titles.size(); i++)
    {
        inputs.push_back(
            ftxui::Container::Horizontal({
                ftxui::Renderer([title = titles[i]] { 
                    return ftxui::text(title + ": "); 
                }),
                ftxui::Input(&m_titles_content[i], titles[i])
            })
        );
    }
    inputs.push_back(ftxui::Button("Добавить", [this]{
        if(m_titles_content[0].empty() || m_titles_content[1].empty() || m_titles_content[2].empty()) 
            return;
        
        m_row_added = hashTable->Add({ m_titles_content });
    }));
    inputs.push_back(ftxui::Renderer([this]{
        return ftxui::text(m_row_added ? "Данные успешно добавлены." : "Не удалось добавить данные в таблицу!");
    }));

    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalHashTableEdit()
{
    auto titles = hashTable->GetTitles();
    m_titles_content.resize(titles.size());
    
    std::vector<ftxui::Component> inputs;
    for(size_t i = 0; i < titles.size(); i++)
    {
        inputs.push_back(
            ftxui::Container::Horizontal({
                ftxui::Renderer([title = titles[i]] { 
                    return ftxui::text(title + ": "); 
                }),
                ftxui::Input(&m_titles_content[i], titles[i])
            })
        );
    }
    inputs.push_back(ftxui::Button("Изменить", [this]{
        m_modified = hashTable->Modify(m_titles_content);
    }));
    inputs.push_back(ftxui::Renderer([this]{
        return ftxui::text(m_modified ? "Данные успешно Изменены." : "Данные не внесены");
    }));
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalHashTableDelete()
{
    auto key_title = hashTable->GetTitles()[0];
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([key_title]{
            return ftxui::text(key_title) | ftxui::vcenter; 
        }),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    inputs.push_back(ftxui::Button("Удалить", [this]{
        m_deleted = hashTable->Delete(m_content);
    }));
    inputs.push_back(ftxui::Renderer([this]{
        return ftxui::text(m_deleted ? "Данные успешно Удалены." : "Данные не удалены");
    }));
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalHashTableFind()
{
    auto titles = hashTable->GetTitles();
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([title = titles[0]]{
            return ftxui::text(title) | ftxui::vcenter; 
        }),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    
    inputs.push_back(ftxui::Button("Найти", [this]{
        m_index = hashTable->Find(m_content);
        if (m_index != -1) {
            *m_record = hashTable->GetDataAt(m_index);
        }
    }));
    
    // ✅ Динамический рендер полей через цикл
    inputs.push_back(ftxui::Renderer([this]{
        if (m_index == -1 || m_record->isEmpty) {
            return ftxui::text("Нет данных") | ftxui::dim;
        }
        
        std::vector<ftxui::Element> elements;
        for (size_t i = 0; i < m_record->fields.size(); ++i) {
            elements.push_back(ftxui::text(m_record->fields[i]));
            // Добавляем разделитель между полями (но не после последнего)
            if (i < m_record->fields.size() - 1) {
                elements.push_back(ftxui::text(" | "));
            }
        }
        return ftxui::hbox(elements);
    }));
    
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalHashTableCollisions()
{
    return ftxui::Renderer([this] {
        auto count = hashTable->Collisions();
        return ftxui::hbox({
            ftxui::text("Коллизии таблицы: "), 
            ftxui::text(std::to_string(count)) | ftxui::bold | ftxui::color(ftxui::Color::Yellow)
        }) | ftxui::center | ftxui::flex;
    });
}