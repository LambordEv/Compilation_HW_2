#ifndef _236360_3_
#define _236360_3_

#include <vector>
#include <string>
#include <iostream>
using namespace std;

namespace output
{
    void endScope();
    void printID(const string &id, int offset, const string &type);
    string makeFunctionType(const string& argType, const string& retType);

    void errorLex(int lineno);
    void errorSyn(int lineno);
    void errorUndef(int lineno, const string &id);
    void errorDef(int lineno, const string &id);
    void errorUndefFunc(int lineno, const string &id);
    void errorMismatch(int lineno);
    void errorPrototypeMismatch(int lineno, const string &id, const string &type);
    void errorUnexpectedBreak(int lineno);
    void errorUnexpectedContinue(int lineno);
    void errorByteTooLarge(int lineno, const string &value);
}

class Node {
public:
    string value;

    Node(const string value = "") : value(value) {
    };
    Node(const Node &node): value(node.value){
    };
    virtual ~Node() = default;
};

#define YYSTYPE Node*

class Program : public Node {
public:
    Program();

    virtual ~Program() = default;
};

class Exp : public Node {
public:
    string type;
    string value;
    bool is_var=false;

    Exp() : type("void"), value("") {}

    Exp(Node *terminal, string type);

    Exp(Node *terminal1, Node *terminal2, const string op, const string type);

    Exp(bool is_var, Node *terminal);

    Exp(Node *exp, Node *type);

    Exp(Exp *exp);

    virtual ~Exp() = default;
};

class Type : public Node {
public:
    string type;

    Type(const string type);

    virtual ~Type() = default;
};

class Call : public Node {
public:
    string type;

    Call(Node *terminal);

    Call(Node *terminal, Node *exp_list);

    virtual ~Call() = default;
};

class Statement : public Node {
public:
    Statement(Node *node);

    Statement(Type *type, Node *id);

    Statement(Type *type, Node *id, Exp *exp);

    Statement(Node *id, Exp *exp);

    Statement(const string name, Exp *exp);

    Statement(Exp *exp, bool is_return=false);

    Statement(Call *call);

    virtual ~Statement() = default;
};

class Statements : public Node {
public:
    Statements(Statement *statement) : Node() {};

    Statements(Statements *statements, Statement *statement) : Node() {};

    virtual ~Statements() = default;
};







#endif