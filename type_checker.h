#ifndef __TYPE_CHECK_H__
#define __TYPE_CHECK_H__
#include <string>
#include "lexer.h"
#include <iostream>
#include <tuple>
using namespace std;

//types of variables
typedef enum{
    INT_T, REAL_T, BOOL_T, NONE_T
}varType;

//types of expressions
typedef enum
{
    UNARY_EXPR, BINARY_EXPR, PRIMARY_EXPR
}exprType;

//types of statements
typedef enum 
{
    IF_S, WHILE_S, SWITCH_S, ASSIGNMENT_S
}stmtType;



//Variable class used to hold information on a variable, including all line numbers where it is encountered
class Variable
{
    public:
        int varNum;
        Variable(string name, varType type, int  line_no);
        string name;
        vector<int> line_nos;
        varType type;
        vector<string> same_type_vars;
        bool printed;
};  

//Expression class represents an expression so that the expression may be type checked later
class Expression
{
    
    public:
        Expression(int line_no, Expression* parent);
        Expression* parent;
        virtual vector<string> getRelatedNames() = 0;
        int line_no;
        exprType type;
        void addVar(vector<Variable> *varlist, Variable v);
        Variable searchVar(vector<Variable>* varlist, string name);
        virtual varType resolveType(vector<Variable>* varlist) = 0;
        void type_mismatch(string miss);
        Expression* copyExpression();

};

//Represents a binary expression, therefore has two other expressions as children
class Binary_Expression: public Expression
{
    public:
        vector<string> getRelatedNames();
        Binary_Expression(int line, Expression* parent);
        TokenType op;
        Expression* expr1, * expr2;
        varType resolveType(vector<Variable>* varlist);
};

//Represents a Unary expression, so it only has one child expression
class Unary_Expression : public Expression
{
    public:
        vector<string> getRelatedNames();
        Unary_Expression(int line, Expression* parent);
        Expression* expr;
        varType resolveType(vector<Variable>* varlist);
};

//Represents a primary expression, which is a terminal for an expression. Therefore, it has no children, instead
//it has the token that represents the terminal.
class Primary_Expression : public Expression
{
    public:
        vector<string> getRelatedNames();
        Primary_Expression(int line, Expression* parent);
        Token primary;
        varType resolveType(vector<Variable>* varlist);
};

//Statement class represents a statment and keeps track of all expressions in the statement.
class Statement
{
    public:
        
        Statement(stmtType name);
        stmtType name;
        vector<Expression*> expressions;
        string relatedName;
        bool finished;
};

//Typer class is main class that types whole program and is active the entire program
class Typer
{
    public:

        void removeDups();
        void validateRelations();
        int isInside(string search);
        void addElement(vector<string> elem);
        void populateRelations();
        void validate();
        Expression* current;
        int line_no;
        Typer();
        void addVar(Variable v);
        void print();
        vector<Variable> sameLineVars;
        vector<Statement> statements;
        void addExpression(Expression* expr);
        Variable searchVar(string name);
        void type_mismatch(string constraint);
        vector<Variable> varList;
        vector<vector<string>> related;
        vector<vector<string>> allRelations;
        vector<Expression*> needToCheckE;
        vector<varType> needsToCheckV;
        vector<int> needsToCheckL;
        vector<string>* error;
        vector<int>* errorLines;
};



#endif