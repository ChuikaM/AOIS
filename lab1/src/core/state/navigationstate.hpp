#pragma once

#include <memory>

class SetTabCommand;

class NavigationState {
public:
    virtual ~NavigationState() = default;
    virtual int getParentTab() const = 0;

};

class MainMenuState : public NavigationState {
public:
    int getParentTab() const override { return -1; }

};

class TableMenuState : public NavigationState {
public:
    int getParentTab() const override { return 0; }

};

class HashTableMenuState : public NavigationState {
public:
    int getParentTab() const override { return 0; }

};

class ModalState : public NavigationState {
protected:
    int m_parentTab;
    
public:
    explicit ModalState(int parentTab) : m_parentTab(parentTab) {}
    int getParentTab() const override { return m_parentTab; }
};

class TablePrintState : public ModalState {
public:
    TablePrintState() : ModalState(1) {}
    
};

class TableAddState : public ModalState {
public:
    TableAddState() : ModalState(1) {}

};

class TableEditState : public ModalState {
public:
    TableEditState() : ModalState(1) {}

};

class TableDeleteState : public ModalState {
public:
    TableDeleteState() : ModalState(1) {}

};

class TableFindState : public ModalState {
public:
    TableFindState() : ModalState(1) {}

};


class HashTablePrintState : public ModalState {
public:
    HashTablePrintState() : ModalState(2) {}

};

class HashTableAddState : public ModalState {
public:
    HashTableAddState() : ModalState(2) {}

};

class HashTableEditState : public ModalState {
public:
    HashTableEditState() : ModalState(2) {}

};

class HashTableDeleteState : public ModalState {
public:
    HashTableDeleteState() : ModalState(2) {}

};

class HashTableFindState : public ModalState {
public:
    HashTableFindState() : ModalState(2) {}

};

class HashTableCollisionsState : public ModalState {
public:
    HashTableCollisionsState() : ModalState(2) {}

};