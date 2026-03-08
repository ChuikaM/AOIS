#pragma once
#include <ftxui/component/component.hpp>

class IMenu 
{
public:
    virtual ~IMenu() = default;
    virtual ftxui::Component CreateMenu() = 0;

};