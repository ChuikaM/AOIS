#pragma once
#include <icommand.hpp>

class SetTabCommand : public ICommand
{
public:
    explicit SetTabCommand(int& tab_ref, int value);
    ~SetTabCommand() override = default;
    void execute() override;

private:
    int& m_tab_ref;
    int m_tab_value {};

};