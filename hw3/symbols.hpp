#ifndef _236360_3
#define _236360_3

#include <string>
#include <vector>
#include <iostream>
#include "hw3_output.hpp"

using std::string;
using std::vector;


string stringToUppercase(const string &str);

class Symbol {
public:
    string name;
    string type;
    int offset;
    bool is_function;
    string param;

    Symbol(const string name, const string type, int offset, bool is_function, string param = string()) : 
        name(name), type(type), offset(offset), is_function(is_function), param(param) {}

    ~Symbol() = default;
};

class SymbolTable {

public:
    vector<Symbol*> symbols;
    int max_offset;
    bool is_loop;
    string* return_type;

    SymbolTable(int offset, bool is_loop, string return_type = "")
            : symbols(), max_offset(offset), is_loop(is_loop) {
        this->return_type = new string(return_type);
    }

    void addSymbolToTable(const Symbol &symbol);

    bool isSymbolExists(const string &name);

    Symbol *getSymbol(const string &name);

    ~SymbolTable(){
        delete return_type;
        for(auto it = symbols.begin(); it != symbols.end(); it++)
            delete (*it);
    }
};

class TableStack {
public:
    vector<SymbolTable*> table_stack;
    vector<int> offsets;

    TableStack();

    void push_scope(bool is_loop = false, string return_type = "");

    SymbolTable *current_scope();

    void pop_scope();

    void addSymbolToStack(const string &name, const string &type, bool is_function, string params = string());

    bool isSymbolExists(const string &name);

    bool isLoop();

    Symbol *getSymbol(const string &name);

    void main();

    ~TableStack() = default;
};

#endif