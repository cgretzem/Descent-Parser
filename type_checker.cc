#include "type_checker.h"


vector<string> errors;
vector<int> errorLinesG;
int varCount = 0;

Variable::Variable(string name, varType type, int line_no)
{
    this->name = name;
    this->line_nos.push_back(line_no);
    this->type = type;
    this->printed = false;
    this->varNum = -1;
}

Statement::Statement(stmtType name)
{
    this->name = name;
    finished = false;

}

Expression::Expression(int line_no, Expression* parent)
{

    if(parent == NULL)
        this->parent = NULL;
    else
        this->parent = parent;

    this->line_no = line_no;
}

//copy of addVar from Typer, does same thing but from scope of expression.
void Expression::addVar(vector<Variable> *varlist, Variable v)
{
    bool found;
    int index;
    for(int i = 0; i < varlist->size(); i++)
    {
        if(varlist->at(i).name == v.name)
        {
            found = true;
            index = i;
        }
    }
    if(found)
    {
        if(varlist->at(index).type == NONE_T || varlist->at(index).same_type_vars != v.same_type_vars)
        {
            v.line_nos.insert(v.line_nos.begin(), varlist->at(index).line_nos.begin(), varlist->at(index).line_nos.end() );
            v.varNum = varlist->at(index).varNum ;
            varlist->at(index) = v;
        }
    }
    else
    {
        v.varNum = varCount;
        varCount++;
        varlist->push_back(v);
    }
}

//copy of searchVar from Typer, does same thing but from scope of expression.
Variable Expression::searchVar(vector<Variable> *varList, string name)
{
    for(int i = 0; i < varList->size(); i++)
    {
        if(varList->at(i).name == name)
        {
            return varList->at(i);
        }
    }
    return Variable(name, NONE_T, line_no);
}


Primary_Expression::Primary_Expression(int t, Expression* parent):Expression(t, parent)
{
    this->type = PRIMARY_EXPR;
}

Binary_Expression::Binary_Expression(int t, Expression* parent):Expression(t, parent)
{
    expr1 = NULL;
    expr2 = NULL;
    this->type = BINARY_EXPR;
}

Unary_Expression::Unary_Expression(int t, Expression* parent):Expression(t, parent)
{
    expr = NULL;
    this->type = UNARY_EXPR;
}



//Resolve type is a core method in the Expression class. It recursively finds the type of any given expression reguardless of how many children it has.
varType Binary_Expression::resolveType(vector<Variable>* varlist)
{
    varType ex1, ex2;
    ex1 = expr1->resolveType(varlist);
    ex2 = expr2->resolveType(varlist);
    if(ex1 == ex2)
    {
        
        if(op == PLUS || op == MINUS || op == MULT || op == DIV)
        {
            return ex1;
        }
            
        else
        {
            return BOOL_T;
        }
            
    }
    else if(ex1 == NONE_T)
    {
        addVar(varlist, Variable(((Primary_Expression*)expr1)->primary.lexeme, ex2, line_no));
        
        return ex2;
    }
       
    else if(ex2 == NONE_T)
    {
        addVar(varlist, Variable(((Primary_Expression*)expr2)->primary.lexeme, ex1, line_no));
        
        return ex1;
    }
    
    else
    {
        errorLinesG.push_back(line_no);
        errors.push_back("C2");
    }
        

    
}


//copys expression to new memory locaiton
Expression* Expression::copyExpression()
{
    Expression* temp;
    switch(type)
    {
         case PRIMARY_EXPR:
            temp = new Primary_Expression(line_no, parent);
            ((Primary_Expression*)temp)->primary =  ((Primary_Expression*)this)->primary;
            break;

        case BINARY_EXPR:
            temp = new Binary_Expression(line_no, parent);
            ((Binary_Expression*)temp)->expr1 = ((Binary_Expression*)this)->expr1->copyExpression();
            ((Binary_Expression*)temp)->expr2 = ((Binary_Expression*)this)->expr2->copyExpression();
            ((Binary_Expression*)temp)->expr1->parent = temp;
            ((Binary_Expression*)temp)->expr2->parent = temp;
            ((Binary_Expression*)temp)->op = ((Binary_Expression*)this)->op;
            break;

        case UNARY_EXPR:
            temp = new Unary_Expression(line_no, parent);
            ((Unary_Expression*)temp)->expr = ((Unary_Expression*)this)->expr->copyExpression();
            ((Unary_Expression*)temp)->expr->parent = temp;
            break;
    }
    temp->type = type;
    return temp;
}

varType Unary_Expression::resolveType(vector<Variable>* varlist)
{
   if(expr->resolveType(varlist) == NONE_T && expr->type == PRIMARY_EXPR)
   {
       addVar(varlist, Variable(((Primary_Expression*)expr)->primary.lexeme, BOOL_T, line_no));
   }
    varType temp = expr->resolveType(varlist);
    if(temp != BOOL_T)
    {
        errorLinesG.push_back(line_no);
        errors.push_back("C3");
    }
        
    else
        return BOOL_T;
        
}

varType Primary_Expression::resolveType(vector<Variable>* varlist)
{
    switch(primary.token_type)
    {
        case ID:
            return searchVar(varlist, primary.lexeme).type;
            break;
        case NUM:
            return INT_T;
            break;
        case REALNUM:
            return REAL_T;
            break;
        case TRUE:
            return BOOL_T;
            break;
        case FALSE:
            return BOOL_T;
            break;
    }
    
}
// simple helper method for printing
void Expression::type_mismatch(string constraint)
{
    cout << "TYPE MISMATCH " << line_no << " " << constraint << endl;
    exit(1);
}

//getRelatedNames is used for type-checking at the end of parsing to make sure that no semantic errors occur and to print output correctly
//Retrieves all variables in the expression that have the same type.
vector<string> Binary_Expression::getRelatedNames()
{
    vector<string> output;
    vector<string> ch1 = expr1->getRelatedNames();
    vector<string> ch2 = expr2->getRelatedNames();
    output.insert(output.end(), ch1.begin(), ch1.end());
    output.insert(output.end(), ch2.begin(), ch2.end());
    return output;

}

vector<string> Unary_Expression::getRelatedNames()
{
    vector<string> output;
    vector<string> ch1 = expr->getRelatedNames();
    output.insert(output.end(), ch1.begin(), ch1.end());
    return output;
}

vector<string> Primary_Expression::getRelatedNames()
{
    vector<string> output;
    if(primary.token_type == ID)
        output.push_back(primary.lexeme);
    return output;
}

//expressions need type
//expressions are composed of sub expressions
Typer::Typer()
{
    this->line_no = 1;
    this->current = NULL;
    this->error = &errors;
    this->errorLines = &errorLinesG;
}


//adds a variable ot the varList, and if the variable name is already in the list, updates that entry in the list.
void Typer::addVar(Variable v)
{
    bool found;
    int index;
    for(int i = 0; i < this->varList.size(); i++)
    {
        if(this->varList[i].name == v.name)
        {
            found = true;
            index = i;
        }
    }
    if(found)
    {
        if(this->varList[index].type == NONE_T || varList[index].same_type_vars != v.same_type_vars)
        {
            v.line_nos.insert(v.line_nos.begin(), varList.at(index).line_nos.begin(), varList.at(index).line_nos.end() );
            v.varNum = varList.at(index).varNum ;
            this->varList[index] = v;
        }
    }
    else
    {
        v.varNum = varCount;
        varCount++;
        this->varList.push_back(v);
    }
    
}

//checks is string is inside allRelations
int Typer::isInside(string search)
{
    for(int i = 0; i < allRelations.size(); i++)
    {
        for(int j = 0; j < allRelations[i].size(); j++)
        {
            if(allRelations[i][j] == search)
            {
                return i;
            }
        }
    }
    return -1;
}



//remove duplicate relations in the relationlist
void Typer::removeDups()
{
    vector<string> uniques;
    for(int k = 0; k < allRelations.size(); k++)
    {
        
        for(int i = 0; i < allRelations[k].size(); i++)
        {
            string search = allRelations[k].at(i);
            bool found = false;
            for(int j =0; j < uniques.size(); j++)
            {
                
                if(uniques[j] == search)
                {
                    found = true;
                }
                
            }
            if(found)
            {
                allRelations[k].erase(allRelations[k].begin()+i);
                i--;
            }
                   
            else
            {
                uniques.push_back(search);
            }
                
        }
        uniques.clear();
    }
}
// adds an element to the relationlist in the relation it is supposed to go into
void Typer::addElement(vector<string> elem)
{
    bool added = false;
    for(int i = 0; i < elem.size(); i++)
    {
        if(isInside(elem[i]) != -1)
        {
            allRelations[isInside(elem[i])].insert(allRelations[isInside(elem[i])].end(), elem.begin(), elem.end());


            added = true;
        }
    }
    if(!added)
        allRelations.push_back(elem);

}

//helper method to check if a string is inside two vectors
bool hasMatches(vector<string> v1, vector<string>v2)
{
    bool match = false;
    for(int i = 0; i < v1.size(); i++)
    {
        for(int j = 0; j < v2.size(); j++)
        {
            if(v1[i] == v2[j])
                match = true;
        }
    }
    return match;
}


//long method to make sure that all the relations are sorted, and each variable is exclusive to one relation.
void Typer::validateRelations()
{
    string search = "";
    for(int i = 0; i < allRelations.size(); i++)
    {
        for(int j = i+1; j < allRelations.size(); j++)
        {
            if(hasMatches(allRelations[i], allRelations[j]))
            {
                allRelations[i].insert(allRelations[i].end(), allRelations[j].begin(), allRelations[j].end());
                allRelations.erase(allRelations.begin() + j);
                removeDups();
            }
        }
    }
    for(int i = 0; i < allRelations.size(); i++)
    {
        for(int j = 0; j < allRelations[i].size(); j++)
        {
            for(int k = j; k < allRelations[i].size(); k++)
            {
                if(searchVar(allRelations[i][j]).varNum > searchVar(allRelations[i][k]).varNum)
                {
                    string temp = allRelations[i][j];
                    allRelations[i][j] = allRelations[i][k];
                    allRelations[i][k] = temp;
                }
            }
           
        }
    }
    
    for(int i = 0; i < allRelations.size(); i++)
    {
        varType type = NONE_T;
        bool hasTypes = false;
        for(int j = 0; j < allRelations[i].size(); j++)
        {
            if(searchVar(allRelations[i][j]).type != NONE_T)
            {
                hasTypes = true;
                type = searchVar(allRelations[i][j]).type;
            }
        }
         if(hasTypes)
        {
            for(int j = 0; j < allRelations[i].size(); j++)
            {
                addVar( Variable(allRelations[i][j], type, line_no) );
            }
        }
    }
   

}

//adds all variables to the related list
void Typer::populateRelations()
{
    for(int i = 0; i < related.size(); i++)
    {
        for(int j =0; j < related[i].size(); j++)
        {
            if(related[i][j] == "")
            {
                related[i].pop_back();
            }
        }
    }

    for(int i = 0; i < related.size(); i++)
    {
      addElement(related.at(i));
    }
}

//checks if relations are correct, and throws any errors that need to be thrown at the end of the program.
void Typer::validate()
{
    vector<vector<string>> related_copy = related;
    vector<string> allRelated;
    bool notNONE = false;
    varType newType;  
    while(related_copy.size() != 0)
    {
        notNONE = false;
        newType = NONE_T;
       for(int i = 0; i < related_copy.back().size(); i++)
       {
           if(searchVar(related_copy.back()[i]).type != NONE_T)
           {
               notNONE = true;
               newType = searchVar(related_copy.back()[i]).type;
           }
       }

       if(notNONE)
       {
           for(int i = 0; i < related_copy.back().size(); i++)
           {
               addVar(Variable(related_copy.back()[i], newType, line_no));
           }
       }
       related_copy.pop_back();
    }

    for(int j = 0; j < errors.size(); j++)
    {
        if(errors[j] == "C4" || errors[j] == "C5")
        {
            for(int i = 0; i < needToCheckE.size(); i++)
            {
            
                if( (needToCheckE[i]->resolveType(&varList)) != needsToCheckV[i])
                {
                    if(needsToCheckV[i] == BOOL_T)
                        cout << "TYPE MISMATCH " << needsToCheckL[i] << " C4"  << endl;
                    else
                        cout << "TYPE MISMATCH " << needsToCheckL[i] << " C5"  << endl;
                    exit(1);
                }
                    
                
            }
        }
        else
        {
            cout << "TYPE MISMATCH " << errorLinesG[j] << " " << errors[j]  << endl;
            exit(1);
        }
            
    }
        
    

}
void Typer::type_mismatch(string constraint)
{
    cout << "TYPE MISMATCH " << line_no << " " << constraint << endl;
    exit(1);
}

Variable Typer::searchVar(string name)
{
    for(int i = 0; i < this->varList.size(); i++)
    {
        if(this->varList[i].name == name)
        {
            return this->varList[i];
        }
    }
    return Variable(name, NONE_T, line_no);
}


//Print method to print either the typing constraint that is violated or print all variables and their types. If some variables have no type and are in the same scope,
//they are printed together.
void Typer::print()
{
    vector<int> printedRelations;
    populateRelations();
    removeDups();
    validateRelations();
    string output = "";
    string line_vars = "";
    for(int i =0; i < varList.size(); i++)
    {
        if(varList[i].type != NONE_T && varList[i].name != "")
        {
            output += varList[i].name + ": ";
            switch(varList[i].type)
            {
                case INT_T:
                    
                    output += "int #\n";
                    break;

                case BOOL_T:
                    output += "bool #\n";
                    break;

                case REAL_T:
                    output += "real #\n";
                    break;
            }
        }
        else
        {    
            bool printRelation = true;
            
            for(int j = 0; j < printedRelations.size(); j++)
            {
                if(printedRelations[j] == isInside(varList[i].name))
                {
                    printRelation = false;
                }
            }
            if(printRelation && isInside(varList[i].name) != -1)
            {
                line_vars+=varList[i].name;
                printedRelations.push_back(isInside(varList[i].name));
                for(int j = 0; j < allRelations[isInside(varList[i].name)].size(); j++)
                {
                    if(searchVar(allRelations[isInside(varList[i].name)][j]).type == NONE_T && allRelations[isInside(varList[i].name)][j] != varList[i].name)
                    {
                        line_vars += ", " + allRelations[isInside(varList[i].name)][j];
                    }
                }
                
                output += line_vars;
                line_vars = "";
                output += ": ? #\n";
            }
        }
    }
    cout << output;
}






//adds a child expression to the current expresssion
void Typer::addExpression(Expression* expr)
{
    if(current == NULL)
    {
        current = expr;
        statements.back().expressions.push_back(current);
        return;
    }
    else if(current->type == UNARY_EXPR)
    {
        expr->parent = current;
        ((Unary_Expression*)current)->expr = expr;
        current = expr;
    }
    else if(current->type == BINARY_EXPR)
    {
        expr->parent = current;
        if(((Binary_Expression*)current)->expr1 == NULL)
        {
            ((Binary_Expression*)current)->expr1 = expr;
        }
        else
            ((Binary_Expression*)current)->expr2 = expr;
        current = expr;
    }

       
}
