#include <mainmenufactory.hpp>

#include <ftxui/component/component.hpp>
#include <settabcommand.hpp>
#include <actioncommand.hpp>

#include <memory>
#include <vector>

ftxui::Component MainMenuFactory::create(OptionType optionType, int& m_current_tab)
{
    switch (optionType)
    {
        case OptionType::MainMenu:
        {
            std::vector<std::pair<std::string, std::unique_ptr<ICommand>>> mainMenuOptions;
            mainMenuOptions.push_back({"Работа с таблицей", std::make_unique<SetTabCommand>(m_current_tab, 1)});
            mainMenuOptions.push_back({"Работа с хэш-таблицей", std::make_unique<SetTabCommand>(m_current_tab, 2)});
            mainMenuOptions.push_back({"Выход", std::make_unique<ActionCommand>([]{})});

            m_mainMenu.emplace(std::move(mainMenuOptions)); 
            return m_mainMenu->CreateMenu();
        }
        case OptionType::RegularTableMenu:
        {
            std::vector<std::pair<std::string, std::unique_ptr<ICommand>>> regularTableMenuOptions;
            regularTableMenuOptions.push_back({"Назад", std::make_unique<SetTabCommand>(m_current_tab, 0)});
            regularTableMenuOptions.push_back({"Вывести таблицу", std::make_unique<SetTabCommand>(m_current_tab, 3)});
            regularTableMenuOptions.push_back({"Добавление", std::make_unique<SetTabCommand>(m_current_tab, 4)});
            regularTableMenuOptions.push_back({"Редактирование", std::make_unique<SetTabCommand>(m_current_tab, 5)});
            regularTableMenuOptions.push_back({"Удаление", std::make_unique<SetTabCommand>(m_current_tab, 6)});
            regularTableMenuOptions.push_back({"Поиск", std::make_unique<SetTabCommand>(m_current_tab, 7)});

            m_regularTableMenu.emplace(std::move(regularTableMenuOptions)); 
            return m_regularTableMenu->CreateMenu();
        }
        case OptionType::HashTableMenu:
        {
            std::vector<std::pair<std::string, std::unique_ptr<ICommand>>> hashTableMenuOptions;
            hashTableMenuOptions.push_back({"Назад", std::make_unique<SetTabCommand>(m_current_tab, 0)});
            hashTableMenuOptions.push_back({"Вывести таблицу", std::make_unique<SetTabCommand>(m_current_tab, 8)});
            hashTableMenuOptions.push_back({"Добавление", std::make_unique<SetTabCommand>(m_current_tab, 9)});
            hashTableMenuOptions.push_back({"Редактирование", std::make_unique<SetTabCommand>(m_current_tab, 10)});
            hashTableMenuOptions.push_back({"Удаление", std::make_unique<SetTabCommand>(m_current_tab, 11)});
            hashTableMenuOptions.push_back({"Поиск", std::make_unique<SetTabCommand>(m_current_tab, 12)});
            hashTableMenuOptions.push_back({"Коллизии", std::make_unique<SetTabCommand>(m_current_tab, 13)});

            m_hashTableMenu.emplace(std::move(hashTableMenuOptions)); 
            return m_hashTableMenu->CreateMenu();
        }
        default:
        {
            return ftxui::Renderer([] { 
                return ftxui::text("Неизвестное действие") | ftxui::dim; 
            });
        }
    }
}