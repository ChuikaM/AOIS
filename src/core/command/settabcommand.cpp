#include <settabcommand.hpp>

SetTabCommand::SetTabCommand(int& tab_ref, int value)
    : m_tab_ref(tab_ref), m_tab_value(value)
{}

void SetTabCommand::execute()
{
    m_tab_ref = m_tab_value;
}