#include "parser.h"
using namespace std;




Parser::Parser()
{
    index = 0;
}
void Parser::pushToVec(Token t)
{
    tokenVec.push_back(t);
    
}

void Parser::syntax_error()
{

   
    cout << "Syntax Error\n";
    exit(1);
}
// peek is used to see what the next token is in the vector without consuming it
Token Parser::peek(int dist)
{
    if(dist <= 0)
    {
        cout << "Peek Error: arg must be > 1\n";
        exit(-1);
    }

    int peekIndex = index + dist -1;
    if(peekIndex > (int) tokenVec.size()-1)
    {
        Token token;
        token.lexeme = "";
        token.line_no = -1;
        token.token_type = END_OF_FILE;
        return token;

    }
    else
    {
        return tokenVec[peekIndex];
    }
    
}
//expect is the core tool of the parser - checks if the next token is what we expect it to be, and if it is, consume it.
//if not, it throws a syntax error
Token Parser::expect(TokenType expected)
{
    Token temp = tokenVec[index];
    if(temp.token_type != expected)
    {
        syntax_error();
    }
    typer.line_no = temp.line_no;
    index++;
    return temp;
}


//This is the entry level of the descent parser - starting and ending here. all parse_x methods are attained through this one.
void Parser::parse_program()
{
    if(peek(1).token_type == ID)
    {
        parse_global_vars();
    }
    parse_body();
    typer.validate();
    typer.print();
    expect(END_OF_FILE);
}


void Parser::parse_global_vars()
{
    parse_var_decl_list();
}

void Parser::parse_var_decl_list()
{
    parse_var_decl();
    if(peek(1).token_type == ID)
    {
        parse_var_decl_list();
    }
}

void Parser::parse_var_decl()
{
    parse_var_list();
    expect(COLON);
    parse_type_name();
    typer.sameLineVars.clear();
    expect(SEMICOLON);
    
}

void Parser::parse_var_list()
{

    typer.sameLineVars.push_back(Variable(expect(ID).lexeme, NONE_T, typer.line_no));
    if(peek(1).token_type == COMMA)
    {
        expect(COMMA);
        parse_var_list();
    }
}

void Parser::parse_type_name()
{
    varType type = NONE_T;
    TokenType temp = peek(1).token_type;
    switch(temp)
    {
        case INT:
            expect(INT);
            type = INT_T;
            break;
        case REAL:
            expect(REAL);
            type = REAL_T;
            break;
        case BOOL:
            expect(BOOL);
            type = BOOL_T;
            break;
        default:
            syntax_error();
            break;
    }
    //pushing all the variable declarations into the typer's var list
    for(int i = 0; i < typer.sameLineVars.size(); i++)
    {
        typer.sameLineVars[i].type = type;
        typer.addVar(typer.sameLineVars[i]);
    }
}
void Parser::parse_body()
{
    expect(LBRACE);
    parse_statement_list();
    expect(RBRACE);
}

void Parser::parse_statement_list()
{
    parse_statement();
    TokenType temp = peek(1).token_type;
    if( temp == ID || temp == IF || temp == WHILE || temp == SWITCH)
    {
        parse_statement_list();
    }
}

void Parser::parse_statement()
{
    TokenType temp = peek(1).token_type;
    switch(temp)
    {
        //typer.statements refers to a list of statments being used as a stack to keep track of the current scope.
        case ID:
            
            parse_assignment_stmt();
            break;
        case IF:
            typer.statements.push_back(Statement(IF_S));
            parse_if_stmt();
            typer.statements.pop_back();
            
            
            break;
        case WHILE:
            typer.statements.push_back(Statement(WHILE_S));
            parse_while_stmt();
            typer.statements.pop_back();
            
            break;
        case SWITCH:
            typer.statements.push_back(Statement(SWITCH_S));
            parse_switch_stmt();
            typer.statements.pop_back();
            
            break;
        default:
            syntax_error();
            break;
    }
    
}

void Parser::parse_assignment_stmt()
{
    string LHS = expect(ID).lexeme;
    //if variable is not in list already or it doesnt have a type, assign it the type of the RHS of the assignment statement
    if(typer.searchVar(LHS).type == NONE_T)
    {
         typer.addVar(Variable(LHS, NONE_T, typer.line_no));
    }
   
    expect(EQUAL);
    typer.statements.push_back(Statement(ASSIGNMENT_S));
    typer.statements.back().relatedName = LHS;
    parse_expression();

   
  
    if(typer.searchVar(LHS).type == NONE_T)
    {
        //setting temp to the current expressions type, which is stored in the current statement's expression list.
        Variable temp = Variable(LHS, typer.statements.back().expressions.front()->resolveType(&(typer.varList)), typer.line_no);
        typer.addVar(temp);

    
    }
    else
    {
        if(typer.searchVar(LHS).type != typer.statements.back().expressions.front()->resolveType(&(typer.varList)))
        {
            typer.error->push_back("C1");
            typer.errorLines->push_back(typer.line_no);

        }
    }

    typer.statements.pop_back();
    
    expect(SEMICOLON);
}

void Parser::parse_expression()
{


    TokenType temp = peek(1).token_type;
    

    if(temp == ID || temp == NUM || temp == REALNUM || temp == TRUE || temp == FALSE)
    {
        
        parse_primary();
        if(typer.current->parent == NULL) //checking if it the last or first expression in the list

        {
            
            typer.related.push_back(typer.current->getRelatedNames());
            typer.related.back().push_back(typer.statements.back().relatedName);   
        }
        typer.current = typer.current->parent;
    }
    else if(temp == NOT)
    {
        parse_unary_operator();
        parse_expression();
        typer.current = typer.current->parent;
    
        
    }
    else
    {
        parse_binary_operator();
        parse_expression();
        parse_expression();
        
      
        typer.related.push_back(typer.current->getRelatedNames());
        if(typer.current->parent == NULL)
        {
            typer.related.back().push_back(typer.statements.back().relatedName);   
        }
    
        typer.current = typer.current->parent;
      
    }
}

void Parser::parse_unary_operator()
{
    typer.addExpression(new Unary_Expression(typer.line_no, NULL));
    expect(NOT);
}

void Parser::parse_binary_operator()
{
    TokenType temp = peek(1).token_type;
    Binary_Expression* expr = new Binary_Expression(typer.line_no, NULL);
    switch(temp)
    {
        case PLUS:
            expr->op = expect(PLUS).token_type;
            break;
        case MINUS:
            expr->op = expect(MINUS).token_type;
            break;
        case MULT:
            expr->op = expect(MULT).token_type;
            break;
        case DIV:
            expr->op = expect(DIV).token_type;
            break;
        case GREATER:
            expr->op = expect(GREATER).token_type;
            break;
        case LESS:
            expr->op = expect(LESS).token_type;
            break;
        case GTEQ:
            expr->op = expect(GTEQ).token_type;
            break;
        case LTEQ:
            expr->op = expect(LTEQ).token_type;
            break;
        case EQUAL:
            expr->op = expect(EQUAL).token_type;
            break;
        case NOTEQUAL:
            expr->op = expect(NOTEQUAL).token_type;
            break;
        default:
            syntax_error();
            break;
    }
    typer.addExpression(expr);
}

void Parser::parse_primary()
{
    TokenType temp = peek(1).token_type;
    if(temp == ID || temp == NUM || temp == REALNUM || temp == TRUE || temp == FALSE)
    {
        Token RHS = expect(temp);

       
        if(RHS.token_type == ID)
        {
            if(typer.searchVar(RHS.lexeme).type == NONE_T)
            {
                
                typer.addVar(Variable(RHS.lexeme, NONE_T, typer.line_no));
            }
               
        }
        Primary_Expression* expr = new Primary_Expression(typer.line_no, NULL);
        expr->primary = RHS;
        typer.addExpression(expr);
    }
    
    else
    {
        syntax_error();
    }
}

void Parser::parse_if_stmt()
{
    expect(IF);
    expect(LPAREN);
    
    parse_expression();
    if(typer.statements.back().expressions.front()->resolveType(&(typer.varList)) != BOOL_T)
        {
            if(typer.statements.back().expressions.front()->type == PRIMARY_EXPR && typer.statements.back().expressions.front()->resolveType(&(typer.varList)) == NONE_T)
            {
                Variable v( ((Primary_Expression*)typer.statements.back().expressions.front())->primary.lexeme, BOOL_T, typer.statements.back().expressions.front()->line_no);
                typer.addVar(v);
            }
            else
            {
                 Expression * front = typer.statements.back().expressions.front();
                Expression* temp = front->copyExpression();

                typer.error->push_back("C4");
                typer.errorLines->push_back(typer.line_no);
                typer.needToCheckE.push_back(temp);
                typer.needsToCheckV.push_back(BOOL_T);
                typer.needsToCheckL.push_back(typer.line_no);
            }
           
            //typer.type_mismatch("C4");
        }
    expect(RPAREN);
    parse_body();
    
}

void Parser::parse_while_stmt()
{
    expect(WHILE);
    expect(LPAREN);
    parse_expression();
    if(typer.statements.back().expressions.front()->resolveType(&(typer.varList)) != BOOL_T)
    {
            if(typer.statements.back().expressions.front()->type == PRIMARY_EXPR)
            {
                typer.addVar(Variable(  ((Primary_Expression*)typer.statements.back().expressions.front()->type)->primary.lexeme, BOOL_T, typer.statements.back().expressions.front()->line_no) );
            }
            else
            {
                Expression * front = typer.statements.back().expressions.front();
                Expression* temp = front->copyExpression();

                typer.error->push_back("C4");
                typer.errorLines->push_back(typer.line_no);
                typer.needToCheckE.push_back(temp);
                typer.needsToCheckV.push_back(BOOL_T);
                typer.needsToCheckL.push_back(typer.line_no);
            }
            
        //typer.type_mismatch("C4");
    }
    expect(RPAREN);
    parse_body();
}

void Parser::parse_switch_stmt()
{
    expect(SWITCH);
    expect(LPAREN);
    parse_expression();
    if(typer.statements.back().expressions.front()->resolveType(&(typer.varList)) != INT_T)
        {
            if(typer.statements.back().expressions.front()->type == PRIMARY_EXPR)
            {
                typer.addVar(Variable(  ((Primary_Expression*)typer.statements.back().expressions.front()->type)->primary.lexeme, BOOL_T, typer.statements.back().expressions.front()->line_no) );
            }
            else
            {
                Expression * front = typer.statements.back().expressions.front();
                Expression* temp = front->copyExpression();

                typer.error->push_back("C5");
                typer.errorLines->push_back(typer.line_no);
                typer.needToCheckE.push_back(temp);
                typer.needsToCheckV.push_back(INT_T);
                typer.needsToCheckL.push_back(typer.line_no);
            }
            
            //typer.type_mismatch("C5");
        }
    expect(RPAREN);
    expect(LBRACE);
    parse_case_list();
    expect(RBRACE);
}

void Parser::parse_case_list()
{
    parse_case();
    if(peek(1).token_type == CASE)
    {
        parse_case_list();
    }
}

void Parser::parse_case()
{
    expect(CASE);
    expect(NUM);
    expect(COLON);
    parse_body();
}



