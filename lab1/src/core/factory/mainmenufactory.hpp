#pragma once
#include <ftxui/component/component.hpp>
#include <optional>
#include <menu.hpp>

enum OptionType {
    MainMenu,
    RegularTableMenu,
    HashTableMenu
};

class MainMenuFactory {
public:
    ftxui::Component create(OptionType optionType, int& m_current_tab);

private:
    std::optional<Menu> m_mainMenu; 
    std::optional<Menu> m_regularTableMenu;
    std::optional<Menu> m_hashTableMenu;

};