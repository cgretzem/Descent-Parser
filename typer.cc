#include "typer.h"
using namespace std;



vector<Variable*> Typer::varList;
vector<vector<Variable*>> Typer::relatedVars;
vector<check*> Typer::exprToCheck;
string Typer::errorType = "";
int Typer::error_line = -1;

int Typer::searchVarList(string var)
{
    for(int i = 0; i < varList.size(); i++)
    {
        if(varList[i]->getLexeme() == var)
            return i;
    }
    return -1;
}

void Typer::type_mismatch(string constraint, int line)
{
    cout <<"TYPE MISMATCH " << line <<" " << constraint << endl;
    exit(1);
}

void Typer::set_var_types(vector<Variable*> vec, varType newType)
{
    for(Variable* v : vec)
    {
        v->setType(newType);
    }
}

int Typer::searchList(vector<Variable*> v1, string item)
{
    for(int i = 0; i < v1.size(); i++)
    {
        if(v1[i]->getLexeme() == item)
            return i;
    }
    return -1;
}

void Typer::relateVars(vector<Variable*> v1, vector<Variable*> v2)
{
    /*
        needs to check if:
        1.v1 has variables already in relatedVars
        2. v2 has variables already in relatedVars
        3. if either of these conditions met, go through related list and
            make list of all lines with at least 1 var from v1 or v2. Remove these ones from related list.
            Add this bigger list back into a new line.

        4. else, add them into a new line
    */
   vector<Variable*> newVec;
   newVec.insert(newVec.end(), v1.begin(), v1.end());
   newVec.insert(newVec.end(), v2.begin(), v2.end());
   vector<Variable*> linesToAdd;
   for(int i = 0; i < relatedVars.size(); i++)
   {
       
       for(Variable* v : newVec)
       {
           if( i >= 0)
            {
                if(searchList(relatedVars[i], v->getLexeme()) != -1)
                {
                    linesToAdd.insert(linesToAdd.end(), relatedVars[i].begin(), relatedVars[i].end());
                    relatedVars.erase(relatedVars.begin() + i);
                    i--;
                }
            }
           
       }
   }
   newVec = combineNoDupes(newVec, linesToAdd);
   relatedVars.push_back(newVec);
}

vector<Variable*> Typer::combineNoDupes(vector<Variable*> v1, vector<Variable*> v2)
{
    vector<Variable*> uniques;
    uniques.insert(uniques.end(), v1.begin(), v1.end());
    for(Variable* v : v2)
    {
        if(searchList(uniques, v->getLexeme()) == -1)
        {
            uniques.push_back(v);
        }
    }
    return uniques;
}


void Typer::backtrack()
{

    for (check* c : exprToCheck)
    {
        if(c->expr->resolveType() != c->t)
        {
            if(errorType != "" && error_line < c->expr->getLineNum())
            {
                type_mismatch(errorType, error_line);
            }
            type_mismatch(c->mismatch,c->expr->getLineNum());
        }
    }
    if(errorType != "")
    {
        type_mismatch(errorType, error_line);
    }

    for(int i = 0; i < relatedVars.size(); i++)
    {
        bool rowFixed = false;
        for(Variable* v : relatedVars[i])
        {
            if(!rowFixed)
            {
                if(v->getType() != NONE_T)
                {
                    set_var_types(relatedVars[i], v->getType());
                    rowFixed = true;
                }
            }
            
        }
    }
}


void Typer::print()
{
    backtrack();
    //print in order of varlist, if type is none print all related vars and set printed to true
    for(int i = 0; i < varList.size();i++)
    {
        Variable* v = varList.at(i);
        if(v->isPrinted() == false)
        {
            if(v->getType() != NONE_T)
            {
                if(v->getType() == REAL_T)
                {
                    cout << v->getLexeme() << ": real #" << endl;
                }
                else if(v->getType() == INT_T)
                {
                    cout << v->getLexeme() << ": int #" << endl;
                }
                else
                    cout << v->getLexeme() << ": bool #" << endl;
                v->makePrinted();
            }
            else
            {
                vector<Variable*> varsToPrint;
                for(vector<Variable*> relation : relatedVars)
                {
                    if(searchList(relation, v->getLexeme()) != -1)
                    {
                        for(Variable* v1 : varList)
                        {
                            if(searchList(relation, v1->getLexeme()) != -1 && v1->isPrinted() == false)
                            {
                                varsToPrint.push_back(v1);
                                v1->makePrinted();
                            }
                        }
                    }
                }
                string output = "";
                for(Variable* v1 : varsToPrint)
                {
                    output += v1->getLexeme() + ", ";
                }
                output.pop_back();
                output.pop_back();
                output += ": ? #";
                cout << output << endl;
            }
        }
        
    }
}