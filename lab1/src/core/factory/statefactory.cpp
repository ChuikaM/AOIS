#include <statefactory.hpp>
#include <memory>
#include <navigationstate.hpp>

std::unique_ptr<NavigationState> StateFactory::createState(int& tabId) {
    switch(tabId) {
        case 0:  return std::make_unique<MainMenuState>();
        case 1:  return std::make_unique<TableMenuState>();
        case 2:  return std::make_unique<HashTableMenuState>();
        
        case 3:  return std::make_unique<TablePrintState>();
        case 4:  return std::make_unique<TableAddState>();
        case 5:  return std::make_unique<TableEditState>();
        case 6:  return std::make_unique<TableDeleteState>();
        case 7:  return std::make_unique<TableFindState>();
        
        case 8:  return std::make_unique<HashTablePrintState>();
        case 9:  return std::make_unique<HashTableAddState>();
        case 10: return std::make_unique<HashTableEditState>();
        case 11: return std::make_unique<HashTableDeleteState>();
        case 12: return std::make_unique<HashTableFindState>();
        case 13: return std::make_unique<HashTableCollisionsState>();
        
        default: return std::make_unique<MainMenuState>();
    }
}
