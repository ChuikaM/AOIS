#include <menu.hpp>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <algorithm>
#include <iterator>

Menu::Menu(std::vector<std::pair<std::string, std::unique_ptr<ICommand>>> options)
    : m_options(std::move(options))
{}

ftxui::Component Menu::CreateMenu()
{
    ftxui::MenuOption opt;
    opt.on_enter = [&] {
        if (m_selected >= 0 && m_selected < static_cast<int>(m_options.size())) {
            m_options[m_selected].second->execute();
        }
    };
   
    std::transform(
        m_options.begin(), 
        m_options.end(), 
        std::back_inserter(m_entries),
        [](const auto& pair) { return pair.first; }
    );
    return ftxui::Menu(&m_entries, &m_selected, opt);
}