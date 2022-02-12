#include "typer.h"
using namespace std;



int Expression::getLineNum()
{
    return line_no;
}


Expression* Expression::getParent()
{
    return parent;
}


//Unary Expression/////////////////////////////////////

Unary_Expression::Unary_Expression(int line, Expression*parent)
{
    this->parent = parent;
    this->line_no = line;
}

Expression* Unary_Expression::getOp()
{
    return op;
}

void Unary_Expression::setOp(Expression* op)
{
    this->op = op;
}

vector<Variable*> Unary_Expression::getVars()
{
    vector<Variable*> newVec;
    newVec.insert(newVec.end(), op->getVars().begin(), op->getVars().end());
    return newVec;
}

varType Unary_Expression::resolveType()
{
    varType type = op->resolveType();
    if(type != BOOL_T)
    {
        if(Typer::errorType == "")
        {
            Typer::errorType = "C3";
            Typer::error_line = line_no;
        }
        //Typer::type_mismatch("C3", line_no);
    }
    return type;
}

//Binary Expression//////////////////////////////////////


Binary_Expression::Binary_Expression(int line, Expression*parent, Token opSymbol)
{
    this->parent = parent;
    this->line_no = line;
    this->opSymbol = opSymbol;
}

Expression* Binary_Expression::getOp1()
{
    return op1;
}

Expression* Binary_Expression::getOp2()
{
    return op2;
}

void Binary_Expression::setOp1(Expression* op1)
{
    this->op1 = op1;
}

void Binary_Expression::setOp2(Expression* op2)
{
    this->op2 = op2;
}

vector<Variable*> Binary_Expression::getVars()
{
    vector<Variable*> newVec;
    vector<Variable*> op1Vars, op2Vars;
    op1Vars = op1->getVars();
    op2Vars = op2->getVars();
    newVec.insert(newVec.end(), op1Vars.begin(), op1Vars.end());
    newVec.insert(newVec.end(), op2Vars.begin(), op2Vars.end());
    return newVec;
}

varType Binary_Expression::resolveType()
{
    varType type1 = op1->resolveType();
    varType type2 = op2->resolveType();
    if(opSymbol.token_type != GTEQ && opSymbol.token_type != GREATER && opSymbol.token_type != LTEQ && opSymbol.token_type != LESS && opSymbol.token_type != EQUAL && opSymbol.token_type != NOTEQUAL)
    {
        if(type1 == NONE_T && type2 == NONE_T)
        {
            Typer::relateVars(op1->getVars(), op2->getVars());
            return NONE_T;
        }
        if(type1 != NONE_T && type2 != NONE_T)
        {
            if(type1 != type2)
            {
                if(Typer::errorType == "")
                {
                    Typer::errorType = "C2";
                    Typer::error_line = line_no;
                }
                //Typer::type_mismatch("C2", line_no);
            }
            return type1;
        }
        if(type1 != NONE_T)
        {
            Typer::set_var_types(op2->getVars(), type1);
            return type1;
        }
        else
        {
            Typer::set_var_types(op1->getVars(), type2);
            return type2;
        }
    }
    else
    {
        if(type1 == NONE_T && type2 == NONE_T)
        {
            Typer::relateVars(op1->getVars(), op2->getVars());
            return BOOL_T;
        }
        if(type1 != NONE_T && type2 != NONE_T)
        {
            if(type1 != type2)
            {
                if(Typer::errorType == "")
                {
                    Typer::errorType = "C2";
                    Typer::error_line = line_no;
                }
                //Typer::type_mismatch("C2", line_no);
            }
            return BOOL_T;
        }
        if(type1 != NONE_T)
        {
            Typer::set_var_types(op2->getVars(), type1);
            return BOOL_T;
        }
        else
        {
            Typer::set_var_types(op1->getVars(), type2);
            return BOOL_T;
        }
    }
        
}


//Primary Expression//////////////////////////////////

Primary_Expression::Primary_Expression(int line, Expression*parent)
{
    this->parent = parent;
    this->line_no = line;
}

Token Primary_Expression::getPrimary()
{
    return primary;
}

void Primary_Expression::setPrimary(Token primary)
{
    this->primary = primary;
}

varType Primary_Expression::resolveType()
{
    if(primary.token_type == ID)
        return Typer::varList[Typer::searchVarList(primary.lexeme)]->getType();
    if(primary.token_type == NUM)
        return INT_T;
    if(primary.token_type == REALNUM)
        return REAL_T;
    return BOOL_T;
}

vector<Variable*> Primary_Expression::getVars()
{
    vector<Variable*> newVec;
    if(primary.token_type == ID)
        newVec.push_back(Typer::varList[Typer::searchVarList(primary.lexeme)]);
    return newVec;
}



