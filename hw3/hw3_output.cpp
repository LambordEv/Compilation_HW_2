#include <iostream>
#include "hw3_output.hpp"
#include "symbols.hpp"
#include <sstream>

using std::vector;
using namespace std;
extern TableStack tables;
extern int yylineno;

void output::endScope(){
    cout << "---end scope---" << endl;
}

void output::printID(const string& id, int offset, const string& type) {
    cout << id << " " << type <<  " " << offset <<  endl;
}

string output::makeFunctionType(const string& argType, const string& retType) {
    
    return "(" + argType + ")" + "->" + retType;
}

void output::errorLex(int lineno){
    cout << "line " << lineno << ":" << " lexical error" << endl;
}

void output::errorSyn(int lineno){
    cout << "line " << lineno << ":" << " syntax error" << endl;
}

void output::errorUndef(int lineno, const string& id){
    cout << "line " << lineno << ":" << " variable " << id << " is not defined" << endl;
}

void output::errorDef(int lineno, const string& id){
    cout << "line " << lineno << ":" << " identifier " << id << " is already defined" << endl;
}

void output::errorUndefFunc(int lineno, const string& id) {
    cout << "line " << lineno << ":" << " function " << id << " is not defined" << endl;
}

void output::errorMismatch(int lineno){
    cout << "line " << lineno << ":" << " type mismatch" << endl;
}

void output::errorPrototypeMismatch(int lineno, const string& id, const string &type) {
    cout << "line " << lineno << ": prototype mismatch, function " << id << " expects arguments " << type << endl;
}

void output::errorUnexpectedBreak(int lineno) {
    cout << "line " << lineno << ":" << " unexpected break statement" << endl;
}

void output::errorUnexpectedContinue(int lineno) {
    cout << "line " << lineno << ":" << " unexpected continue statement" << endl;	
}

void output::errorByteTooLarge(int lineno, const string& value) {
    cout << "line " << lineno << ": byte value " << value << " out of range" << endl;
}

Program::Program() {

}

//************STATEMENT****************
// Statement -> BREAK SC or CONTINUE SC
Statement::Statement(Node *node) {
    if (node->value == "break") {
        if (!tables.check_loop()) {
            output::errorUnexpectedBreak(yylineno);
            exit(0);
        }
    } else if (node->value == "continue") {
        if (!tables.check_loop()) {
            output::errorUnexpectedContinue(yylineno);
            exit(0);
        }
    }
}

Statement::Statement(Exp *exp, bool is_return) : Node() {
    SymbolTable *scope = tables.current_scope();
    string *return_type = scope->return_type;

    if (*return_type != "" && *return_type != exp->type) {
        if (*return_type != "int" || exp->type != "byte") {
            output::errorMismatch(yylineno);
            exit(0);
        }

    }
    if (exp->is_var) {
        Symbol *symbol = tables.get_symbol(exp->value);
        if (symbol->is_function) {
            output::errorUndef(yylineno, symbol->name);
            exit(0);
        }
    }
}

// Statement -> Type ID SC
Statement::Statement(Type *type, Node *id) : Node() {
    if (tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    tables.add_symbol(id->value, type->type, false);
    value = type->value;//

}

static bool check_types_compatible(string type1, string type2) {
    if (type1 == type2)
        return true;

    if (type1 == "bool" || type2 == "bool" || type1 == "string" || type2 == "string" || type2 == "void")
        return false;
    return true;
}

// Statement -> Type ID ASSIGN Exp SC or Statement -> AUTO ID ASSIGN Exp SC
Statement::Statement(Type *type, Node *id, Exp *exp) : Node() {
    if (tables.symbol_exists(id->value)) {
        output::errorDef(yylineno, id->value);
        exit(0);
    }
    if (type) {
        if (!check_types_compatible(type->type, exp->type)) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if (type->type == "byte" && exp->type == "int") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        tables.add_symbol(id->value, type->type, false);
    } else {
        if (exp->type == "void" || exp->type == "string") {
            output::errorMismatch(yylineno);
            exit(0);
        }
        // For Bool expressions
//        if (!check_types_compatible(type->type, exp->type)) {
//            output::errorMismatch(yylineno);
//            exit(0);
//        }
        tables.add_symbol(id->value, exp->type, false);
    }
}

// Statement -> ID ASSIGN Exp SC
Statement::Statement(Node *id, Exp *exp) : Node() {
    if (!tables.symbol_exists(id->value)) {
        output::errorUndef(yylineno, id->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(id->value);
    if (symbol->is_function || !check_types_compatible(symbol->type, exp->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if(symbol->type == "byte" && exp->type == "int"){
        output::errorMismatch(yylineno);
        exit(0);
    }
}

// Statement -> Call SC
Statement::Statement(Call *call) : Node() {
    if (!tables.symbol_exists(call->value)) {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(call->value);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, call->value);
        exit(0);
    }
}

Statement::Statement(const string name, Exp *exp) {
    if (exp->type != "bool") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

Type::Type(const string type) : Node(), type(type) {

}

// ***************EXP******************
// Exp -> LPAREN Exp RPAREN
Exp::Exp(Exp *exp) : Node(exp->value), type(exp->type) {
}

// Exp -> CONST(bool, num, byte, string)
Exp::Exp(Node *terminal, string
type) : Node(terminal->value), type(type) {
    if (type == "byte") {
        int value = stoi(terminal->value);
        if (value > 255) {
            output::errorByteTooLarge(yylineno, terminal->value);
            exit(0);
        }
    }
}

//Exp -> ID, Call
Exp::Exp(bool is_var, Node *terminal) : Node(), is_var(is_var) {
    if (is_var && !tables.symbol_exists(terminal->value)) {
        output::errorUndef(yylineno, terminal->value);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(terminal->value);
    value = terminal->value;
    type = symbol->type;
}

Exp::Exp(Node *terminal1, Node *terminal2,
         const string op,
         const string type) {
    Exp *exp1 = dynamic_cast<Exp *>(terminal1);
    Exp *exp2 = dynamic_cast<Exp *>(terminal2);
    if (exp1->is_var && !tables.symbol_exists(exp1->value)) {
        output::errorUndef(yylineno, terminal1->value);
        exit(0);
    }

    if (exp2->is_var && !tables.symbol_exists(exp2->value)) {
        output::errorUndef(yylineno, terminal2->value);
        exit(0);
    }

    if (!check_types_compatible(exp1->type, exp2->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    if (type == "bool") {
//        bool t1, t2, res;
        if (exp1->type != "bool" || exp2->type != "bool") {
            output::errorMismatch(yylineno);
            exit(0);
        }

        this->type = "bool";
    } else if (type == "int") {

        if ((exp1->type != "int" && exp1->type != "byte") || (exp1->type != "int" && exp1->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }

        if (exp1->type == "int" || exp2->type == "int") {
            this->type = "int";
        } else {
            this->type = "byte";
        }
    } else if (type == "relop") {
        if (!check_types_compatible(exp1->type, exp2->type)) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        if ((exp1->type != "int" && exp1->type != "byte") || (exp1->type != "int" && exp1->type != "byte")) {
            output::errorMismatch(yylineno);
            exit(0);
        }
        this->type = "bool";
    }
}

// Exp -> LPAREN Type RPAREN Exp
Exp::Exp(Node *exp, Node *type) {
    Exp *converted_exp = dynamic_cast<Exp *>(exp);
    Type *converted_type = dynamic_cast<Type *>(type);

    if (!check_types_compatible(converted_exp->type, converted_type->type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    this->value = converted_exp->value;
    this->type = converted_type->type;
}


// Call -> ID LPAREN Exp RPAREN
Call::Call(Node *terminal, Node *exp) : Node() {
    Exp *expression = dynamic_cast<Exp *>(exp);
    string name = terminal->value;
    if (!tables.symbol_exists(name)) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
    Symbol *symbol = tables.get_symbol(name);
    if (!symbol->is_function) {
        output::errorUndefFunc(yylineno, name);
        exit(0);
    }
    for (int i = 0; i < symbol->params.size(); i++) {
        if (symbol->params[i] != expression->type) {
            if (symbol->params[i] != "int" || expression->type != "byte") {
                /*vector<string> converted_params = vector<string>();
                for (int j = 0; j < symbol->params.size(); ++j) {
                    converted_params.push_back(convert_to_upper_case(symbol->params[j]));
                }*/
                output::errorPrototypeMismatch(yylineno, name, "byte");
                exit(0);
            }
        }
    }
    type = symbol->type;
    value = symbol->name;
}
