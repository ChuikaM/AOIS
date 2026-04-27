#include <app.hpp>

#include <ftxui/component/screen_interactive.hpp>

#include <settabcommand.hpp>

#include <modalfactory.hpp>
#include <mainmenufactory.hpp>

#include <itable.hpp>
#include <regulartable.hpp>
#include <hashtable.hpp>
#include <fstream>

App::App()
{
    m_regularTable = std::make_unique<RegularTable>(64);
    m_regularTable->loadTable("./files/file.csv");
   
    m_hashTable = std::make_unique<HashTable>(64);
    m_hashTable->loadTable("./files/file.csv");

    m_found_record = std::make_unique<Record>();
}
App::~App() = default;

void App::Run()
{
    auto screen = ftxui::ScreenInteractive::TerminalOutput();
    
    auto mainMenuFactory = std::make_unique<MainMenuFactory>();
    auto modalFactory = std::make_unique<ModalFactory>();
    ModalFactory::Context ctx;
    ctx.input_fields = &m_form_inputs;
    ctx.search_key = &m_search_key;
    ctx.operation_success = &m_operation_ok;
    ctx.feedback_message = &m_feedback;

    auto tabs = ftxui::Container::Tab({
        mainMenuFactory->create(OptionType::MainMenu, m_current_tab),
        mainMenuFactory->create(OptionType::RegularTableMenu, m_current_tab),
        mainMenuFactory->create(OptionType::HashTableMenu, m_current_tab),
        
        modalFactory->create(TableType::Regular, ModalAction::Print, m_regularTable.get(), ctx),
        modalFactory->create(TableType::Regular, ModalAction::Add, m_regularTable.get(), ctx),
        modalFactory->create(TableType::Regular, ModalAction::Edit, m_regularTable.get(), ctx),
        modalFactory->create(TableType::Regular, ModalAction::Delete, m_regularTable.get(), ctx),
        modalFactory->create(TableType::Regular, ModalAction::Find, m_regularTable.get(), ctx),
    
        modalFactory->create(TableType::Hash, ModalAction::Print, m_hashTable.get(), ctx),
        modalFactory->create(TableType::Hash, ModalAction::Add, m_hashTable.get(), ctx),
        modalFactory->create(TableType::Hash, ModalAction::Edit, m_hashTable.get(), ctx),
        modalFactory->create(TableType::Hash, ModalAction::Delete, m_hashTable.get(), ctx),
        modalFactory->create(TableType::Hash, ModalAction::Find, m_hashTable.get(), ctx),
        modalFactory->create(TableType::Hash, ModalAction::Collisions, m_hashTable.get(), ctx),
        modalFactory->create(TableType::Hash, ModalAction::Graph, m_hashTable.get(), ctx)
    }, &m_current_tab);

    std::vector<std::string> titles {
        "Главное меню", "Работа с таблицей", "Работа с хэш-таблицей", 
        "Вывод таблицы", "Добавление в таблицу", "Редактирование таблицы", "Удаление в таблице", "Поиск в таблице",
        "Вывод хэш-таблицы", "Добавление в хэш-таблицу", "Редактирование хэш-таблицы", "Удаление в хэш-таблице", "Поиск в хэш-таблице", "Коллизии", "График"
    };
    auto renderer = ftxui::Renderer(tabs, [&] {
        return ftxui::vbox({
            ftxui::text(titles[m_current_tab]) | ftxui::bold | ftxui::center,
            ftxui::separator(),
            tabs->Render() | ftxui::flex,
            ftxui::separator(),
            ftxui::text(m_current_tab == 0 ? "Enter: Выбор" : "Enter: Выбор | Esc: Назад") | ftxui::dim | ftxui::center,
        }) | ftxui::border | ftxui::flex;
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