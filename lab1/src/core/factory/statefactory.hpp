#pragma once
#include <memory>

class NavigationState;

class StateFactory {
public:
    static std::unique_ptr<NavigationState> createState(int& tabId);

};