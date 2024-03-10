#include "symbols.hpp"
#include "hw3_output.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <ctype.h>

extern TableStack tables;


void SymbolTable::add_symbol(const Symbol &symbol) {
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


Symbol *SymbolTable::get_symbol(const string &name) {
    for (auto it = symbols.begin(); it != symbols.end(); ++it) {
        if ((*it)->name == name)
            return (*it);
    }
    return nullptr;
}

TableStack::TableStack() : table_stack(), offsets() {
    offsets.push_back(0);
    push_scope(false);
    add_symbol("print", "void", true, {"string"});
    add_symbol("printi", "void", true, {"int"});
    add_symbol("readi", "int", true, {"int"});
}

bool TableStack::isSymbolExists(const string &name) {
    for (auto it = table_stack.rbegin(); it != table_stack.rend(); ++it) {
        SymbolTable *current = *it;
        if (current->isSymbolExists(name))
            return true;
    }
    return false;
}

bool TableStack::check_loop() {
    for (auto it = table_stack.rbegin(); it != table_stack.rend(); ++it) {
        SymbolTable *current = *it;
        if (current->is_loop)
            return true;
    }
    return false;
}
Symbol *TableStack::get_symbol(const string &name) {
    for (auto it = table_stack.begin(); it != table_stack.end(); ++it) {
        Symbol *symbol = (*it)->get_symbol(name);
        if (symbol)
            return symbol;
    }
    return nullptr;
}

void TableStack::add_symbol(const string &name, const string &type, bool is_function, string param) {
    SymbolTable *current_scope = table_stack.back();
    int offset;
    if (is_function) {
        offset = 0;
    } else {
        offset = offsets.back();
        offsets.push_back(offset + 1);
    }

    Symbol symbol = Symbol(name, type, offset, is_function, param);
    current_scope->add_symbol(symbol);
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
        auto offset = offsets.back();
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

void TableStack::check_program() {
    SymbolTable *main_scope = tables.table_stack.front();
    if (main_scope->isSymbolExists("main")) {
        Symbol *main_symbol = main_scope->get_symbol("main");
        if (main_symbol->type == "void") {
            if (main_symbol->param == string()) {
                tables.pop_scope();
                return;
            }
        }
    }
    exit(0);
}