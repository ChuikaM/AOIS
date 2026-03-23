#include <actioncommand.hpp>

ActionCommand::ActionCommand(const std::function<void()>& action)
    : m_action(std::move(action))
{}

void ActionCommand::execute()
{
    if(m_action)
    {
        m_action();
    }
}