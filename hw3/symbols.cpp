#include "symbols.hpp"
#include "hw3_output.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

extern TableStack tables;


void SymbolTable::add_symbol(const Symbol &symbol) {
    symbols.push_back(new Symbol(symbol));
    if (symbol.offset >= 0)
        max_offset = symbol.offset;
}


bool SymbolTable::symbol_exists(const string &name) {
    for(int i = 0; i < symbols.size(); i++){
        if (symbols[i]->name == name)
            return true;
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
}

bool TableStack::symbol_exists(const string &name) {
//    std::cout << "Checking all scopes " << table_stack.size() << "\n";
    for (auto it = table_stack.rbegin(); it != table_stack.rend(); ++it) {
        SymbolTable *current = *it;
        if (current->symbol_exists(name))
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

void TableStack::add_symbol(const string &name, const string &type, bool is_function, vector<string> params) {
    SymbolTable *current_scope = table_stack.back();
    int offset;
    if (is_function) {
        offset = 0;
    } else {
        offset = offsets.back();
        offsets.push_back(offset + 1);
    }

    Symbol symbol = Symbol(name, type, offset, is_function, params);
    current_scope->add_symbol(symbol);
}

void TableStack::add_function_symbol(const string &name, const string &type, int offset) {
    SymbolTable *current_scope = table_stack.back();
    Symbol symbol = Symbol(name, type, offset, false, vector<string>());
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

string convert_to_upper_case(const string &str) {
    if (str == "bool")
        return "BOOL";
    else if (str == "byte")
        return "BYTE";
    else if (str == "int")
        return "INT";
    else if (str == "void")
        return "VOID";
    else
        return "STRING";
}

std::string concatenateParams(const std::vector<std::string>& params) {
    std::ostringstream oss;
    std::copy(params.begin(), params.end(), std::ostream_iterator<std::string>(oss, ", "));
    std::string result = oss.str();
    // Remove the trailing comma and space
    if (!result.empty()) {
        result.pop_back();
        result.pop_back();
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
            vector<string> converted_params;
            for (int i = 0; i < (*it)->params.size(); ++i) {
                converted_params.push_back(convert_to_upper_case((*it)->params[i]));
            }
            std::string paramsString = concatenateParams(converted_params);
            output::printID((*it)->name, 0, output::makeFunctionType(convert_to_upper_case((*it)->type), paramsString));
        } else {
            output::printID((*it)->name, (*it)->offset, convert_to_upper_case((*it)->type));
        }
    }
    if(offsets.size() > 0)
        offsets.pop_back();
    delete scope;
}

void TableStack::print_scopes() {
    int i = 0;
    for (auto it = table_stack.begin(); it != table_stack.end(); ++it) {
        std::cout << "Scope Number:" << i << " Return Type: " << *((*it)->return_type) << " Number of symbols:"
                  << (*it)->symbols.size() << std::endl;
        for (auto it2 = (*it)->symbols.begin(); it2 != (*it)->symbols.end(); ++it2) {
            std::cout << (*it2)->name << " " << (*it2)->type << " " << (*it2)->offset << std::endl;
        }
        i++;
    }
}

void TableStack::check_program() {
    SymbolTable *main_scope = tables.table_stack.front();
    if (main_scope->symbol_exists("main")) {
        Symbol *main_symbol = main_scope->get_symbol("main");
        if (main_symbol->type == "void") {
            if (main_symbol->params.size() == 0) {
                tables.pop_scope();
                return;
            }
        }
    }
    exit(0);
}