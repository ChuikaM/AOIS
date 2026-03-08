#pragma once
#include <icommand.hpp>
#include <functional>

class ActionCommand : public ICommand
{
public:
    explicit ActionCommand(const std::function<void()>& action);
    ~ActionCommand() override = default;

    void execute() override;

private:
    std::function<void()> m_action;

};