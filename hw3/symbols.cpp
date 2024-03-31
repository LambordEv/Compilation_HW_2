#include "symbols.hpp"
#include "hw3_output.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <ctype.h>

extern TableStack mainStack;


void SymbolTable::addSymbolToTable(const Symbol &symbol) {
    symbols.push_back(new Symbol(symbol));
    if (symbol.offset >= 0)
        max_offset = symbol.offset;
}


bool SymbolTable::isSymbolExists(const string &name) {
    for(auto it = this->symbols.begin(); this->symbols.end() != it; ++it){
        if(name == (*it)->name){
            return true;
        }
    }
    return false;
}


Symbol *SymbolTable::getSymbol(const string &name) {
    for (auto it = symbols.begin(); it != symbols.end(); ++it) {
        if ((*it)->name == name)
            return (*it);
    }
    return nullptr;
}

TableStack::TableStack() : table_stack(), offsets() {
    offsets.push_back(0);
    push_scope(false);
    addSymbolToStack("print", "void", true, {"string"});
    addSymbolToStack("printi", "void", true, {"int"});
    addSymbolToStack("readi", "int", true, {"int"});
}

bool TableStack::isSymbolExists(const string &name) {
    for (auto it = table_stack.rbegin(); it != table_stack.rend(); ++it) {
        SymbolTable *current = *it;
        if (current->isSymbolExists(name))
            return true;
    }
    return false;
}

bool TableStack::isLoop() {
    for (auto it = table_stack.rbegin(); it != table_stack.rend(); ++it) {
        SymbolTable *current = *it;
        if (current->is_loop)
            return true;
    }
    return false;
}

Symbol *TableStack::getSymbol(const string &name) {
    for (auto it = table_stack.begin(); it != table_stack.end(); ++it) {
        Symbol *symbol = (*it)->getSymbol(name);
        if (symbol)
            return symbol;
    }
    return nullptr;
}

void TableStack::addSymbolToStack(const string &name, const string &type, bool is_function, string param) {
    SymbolTable *current_scope = table_stack.back();
    int order;
    if (is_function) {
        order = 0;
    } else {
        order = offsets.back();
        offsets.push_back(order + 1);
    }
    Symbol symbol = Symbol(name, type, order, is_function, param);
    current_scope->addSymbolToTable(symbol);
}

void TableStack::push_scope(bool is_loop, string return_type) {
    SymbolTable* new_scope = new SymbolTable(offsets.back(), is_loop, return_type);
    this->table_stack.push_back(new_scope);
    SymbolTable* current_scope = table_stack.back();
    offsets.push_back(current_scope->max_offset);
}

SymbolTable *TableStack::current_scope() {
    return table_stack.back();
}

string stringToUppercase(const string &str) {
    string result = string();
    for(auto it = str.begin(); it != str.end(); ++it){
        result += toupper(*it);
    }
    return result;
}

void TableStack::pop_scope() {
    SymbolTable *scope = table_stack.back();
    table_stack.pop_back();
    output::endScope();
    for (auto it = scope->symbols.begin(); it != scope->symbols.end(); ++it) {
        offsets.pop_back();
        if ((*it)->is_function) {
            output::printID((*it)->name, 0, output::makeFunctionType(stringToUppercase((*it)->param), stringToUppercase((*it)->type)));
        } else {
            output::printID((*it)->name, (*it)->offset, stringToUppercase((*it)->type));
        }
    }
    if(offsets.size() > 0)
        offsets.pop_back();
}

void TableStack::main() {
    SymbolTable *firstScope = mainStack.table_stack.front();
    if (firstScope->isSymbolExists("main")) {
        Symbol *sym = firstScope->getSymbol("main");
        if (sym->type == "void") {
            if (sym->param == string()) {
                mainStack.pop_scope();
                return;
            }
        }
    }
    exit(0);
}