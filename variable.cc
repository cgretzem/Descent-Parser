#include "typer.h"
using namespace std;

Variable::Variable(string lexeme, int firstEncounter, varType vartype)
{
    this->lexeme = lexeme;
    this->firstEncounter = firstEncounter;
    this->type = vartype;
    printed = false;
}

varType Variable::getType()
{
    return type;
}

void Variable::setType(varType t)
{
    type = t;
}

string Variable::getLexeme()
{
    return lexeme;
}

int Variable::getFirstEncounter()
{
    return firstEncounter;
}

void Variable::makePrinted()
{
    printed = true;
}

bool Variable::isPrinted()
{
    return printed;
}