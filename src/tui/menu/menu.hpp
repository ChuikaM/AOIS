#pragma once
#include <imenu.hpp>
#include <icommand.hpp>
#include <map>

class Menu : public IMenu
{
public:
    explicit Menu(std::vector<std::pair<std::string, std::unique_ptr<ICommand>>> options);
    ~Menu() override = default;

    ftxui::Component CreateMenu() override;

private:
    int m_selected {};
    std::vector<std::string> m_entries;
    std::vector<std::pair<std::string, std::unique_ptr<ICommand>>> m_options;

};