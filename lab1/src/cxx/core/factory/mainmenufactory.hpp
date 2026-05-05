#pragma once
#include <ftxui/component/component.hpp>
#include <optional>
#include <menu.hpp>

enum class OptionType : unsigned short {
    MainMenu,
    RegularTableMenu,
    HashTableMenu
};

class MainMenuFactory {
public:
    ftxui::Component create(OptionType optionType, int& m_current_tab);

private:
    ftxui::Component m_prepareMainMenu(int& m_current_tab);
    ftxui::Component m_prepareRegularTableMenu(int& m_current_tab);
    ftxui::Component m_prepareHashTableMenu(int& m_current_tab);

    std::optional<Menu> m_mainMenu; 
    std::optional<Menu> m_regularTableMenu;
    std::optional<Menu> m_hashTableMenu;

};