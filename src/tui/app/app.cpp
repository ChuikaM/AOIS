#include <app.hpp>

#include <menu.hpp>
#include <settabcommand.hpp>
#include <actioncommand.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <hashtable.hpp>
#include <txttable.hpp>

App::App()
{
    txtTable = std::make_unique<TxtTable>();
    hashTable = std::make_unique<HashTable>();
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
            tabs->Render() | ftxui::vscroll_indicator | ftxui::frame,
            ftxui::separator(),
            ftxui::text(m_current_tab == 0 ? "Enter: Выбор" : "Enter: Выбор | Esc: Назад") | ftxui::dim | ftxui::center,
        }) | ftxui::border;
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
    auto data = txtTable->GetData();
    using namespace ftxui;

    size_t max_width = 0;
    for (const auto& record : data) {
        if (record.isEmpty) continue; 

        max_width = std::max(max_width, record.key.size());
        max_width = std::max(max_width, record.field2.size());
        max_width = std::max(max_width, record.field3.size());
    }
    max_width += 2;

    std::vector<Component> rows;
    for (const auto& record : data) {
        if (record.isEmpty) continue;
        auto row = Container::Horizontal({
            Button(record.key, []{}) | size(WIDTH, EQUAL, max_width),
            Button(record.field2, []{}) | size(WIDTH, EQUAL, max_width),
            Button(record.field3, []{}) | size(WIDTH, EQUAL, max_width)
        });
        rows.push_back(row);
    }
    return Container::Vertical(rows);
}
ftxui::Component App::ModalSimpleTableAdd()
{
    std::vector<std::string> titles = txtTable->GetTitles();
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
        m_row_added = txtTable->Add({ m_titles_content[0], m_titles_content[1], m_titles_content[2]});
    }));
    inputs.push_back(ftxui::Renderer([this]{
        return ftxui::text(m_row_added ? "Данные успешно добавлены." : "Не удалось добавить данные в таблицу!");
    }));

    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalSimpleTableEdit()
{
    m_dropdown_titles = txtTable->GetTitles();
    m_dropdown_selected = 0;
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([]{
            return ftxui::text("Поле: ") | ftxui::vcenter; 
        }),
        ftxui::Dropdown(&m_dropdown_titles, &m_dropdown_selected),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    inputs.push_back(ftxui::Button("Найти", []{}));
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalSimpleTableDelete()
{
    m_dropdown_titles = txtTable->GetTitles();
    m_dropdown_selected = 0;
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([]{
            return ftxui::text("Поле: ") | ftxui::vcenter; 
        }),
        ftxui::Dropdown(&m_dropdown_titles, &m_dropdown_selected),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    inputs.push_back(ftxui::Button("Удалить", []{}));
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalSimpleTableFind()
{
    m_dropdown_titles = txtTable->GetTitles();
    m_dropdown_selected = 0;
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([]{
            return ftxui::text("Поле: ") | ftxui::vcenter; 
        }),
        ftxui::Dropdown(&m_dropdown_titles, &m_dropdown_selected),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    inputs.push_back(ftxui::Button("Найти", []{}));
    return ftxui::Container::Vertical(std::move(inputs));
}


ftxui::Component App::ModalHashTablePrint()
{
    auto data = txtTable->GetTableData();
    using namespace ftxui;

    size_t max_width = 0;
    for (const auto& record : data) {
        if (record.isEmpty) continue; 

        max_width = std::max(max_width, record.key.size());
        max_width = std::max(max_width, record.field2.size());
        max_width = std::max(max_width, record.field3.size());
    }
    max_width += 2;

    std::vector<Component> rows;
    for (const auto& record : data) {
        if (record.isEmpty) continue;
        auto row = Container::Horizontal({
            Button(record.key, []{}) | size(WIDTH, EQUAL, max_width),
            Button(record.field2, []{}) | size(WIDTH, EQUAL, max_width),
            Button(record.field3, []{}) | size(WIDTH, EQUAL, max_width)
        });
        rows.push_back(row);
    }
    return Container::Vertical(rows);
}
ftxui::Component App::ModalHashTableAdd()
{
    std::vector<std::string> titles = hashTable->GetTitles();
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
    inputs.push_back(ftxui::Button("Добавить", []{}));

    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalHashTableEdit()
{
    m_dropdown_titles = hashTable->GetTitles();
    m_dropdown_selected = 0;
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([]{
            return ftxui::text("Поле: ") | ftxui::vcenter; 
        }),
        ftxui::Dropdown(&m_dropdown_titles, &m_dropdown_selected),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    inputs.push_back(ftxui::Button("Найти", []{}));
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalHashTableDelete()
{
    m_dropdown_titles = hashTable->GetTitles();
    m_dropdown_selected = 0;
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([]{
            return ftxui::text("Поле: ") | ftxui::vcenter; 
        }),
        ftxui::Dropdown(&m_dropdown_titles, &m_dropdown_selected),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    inputs.push_back(ftxui::Button("Удалить", []{}));
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalHashTableFind()
{
    m_dropdown_titles = hashTable->GetTitles();
    m_dropdown_selected = 0;
    
    std::vector<ftxui::Component> inputs;
    inputs.push_back(ftxui::Container::Horizontal({
        ftxui::Renderer([]{
            return ftxui::text("Поле: ") | ftxui::vcenter; 
        }),
        ftxui::Dropdown(&m_dropdown_titles, &m_dropdown_selected),
        ftxui::Input(&m_content, "write...") | ftxui::vcenter
    }));
    inputs.push_back(ftxui::Button("Найти", []{}));
    return ftxui::Container::Vertical(std::move(inputs));
}
ftxui::Component App::ModalHashTableCollisions()
{
    auto count = hashTable->Collisions();
    return ftxui::Renderer([count] {
        return ftxui::text("Коллизии таблицы:}"), ftxui::text(std::to_string(count));
    });
}
