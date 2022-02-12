#ifndef __TYPER_H__
#define __TYPER_H__
#include "lexer.h"
#include <iostream>

typedef enum varType
{
    INT_T, REAL_T, BOOL_T, NONE_T
}varType;


class Variable
{
    public:
        Variable(std::string lexeme, int firstEncounter, varType varType);
        varType getType();
        void setType(varType t);
        std::string getLexeme();
        int getFirstEncounter();
        void makePrinted();
        bool isPrinted();
    private:
        bool printed;
        std::string lexeme;
        int firstEncounter;
        varType type;
        
};


class Expression
{
    public:
        virtual varType resolveType() = 0;
        virtual std::vector<Variable*> getVars() = 0;
        int getLineNum();
        Expression* getParent();
    protected:
        int line_no;
        varType type;
        Expression* parent;
};

class Unary_Expression : public Expression
{
    public:
        Unary_Expression(int line, Expression* parent);
        varType resolveType();
        std::vector<Variable*> getVars();

        Expression* getOp();
        void setOp(Expression* op);
    private:
        Expression* op;
};

class Binary_Expression : public Expression
{
    public:
        Binary_Expression(int line, Expression* parent, Token opSymbol);
        varType resolveType();
        std::vector<Variable*> getVars();

        Expression* getOp1();
        Expression* getOp2();
        void setOp1(Expression* op1);
        void setOp2(Expression* op2);

    private:
        Expression* op1;
        Expression* op2;
        Token opSymbol;
};

class Primary_Expression : public Expression
{
    public:
        Primary_Expression(int line, Expression* parent);
        varType resolveType();
        std::vector<Variable*> getVars();

        Token getPrimary();
        void setPrimary(Token primary);
    private:
        Token primary;
};

typedef struct check
{
    Expression* expr;
    varType t;
    std::string mismatch;
}check;


class Typer
{
    
    public:
        static std::string errorType;
        static int error_line;
        static std::vector<Variable*> varList;
        static std::vector<std::vector<Variable*>> relatedVars;
        static std::vector<check*> exprToCheck;
        static int searchVarList(std::string var);
        static void type_mismatch(std::string constraint, int line);
        static void set_var_types(std::vector<Variable*> varsToSet, varType newType);
        static int searchList(std::vector<Variable*> vec, std::string item);
        static void relateVars(std::vector<Variable*> v1, std::vector<Variable*> v2);
        static std::vector<Variable*> combineNoDupes(std::vector<Variable*> v1, std::vector<Variable*> v2);
        static void print();
        static void backtrack();
};

#endif