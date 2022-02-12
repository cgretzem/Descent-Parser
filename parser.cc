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

Token Parser::expect(TokenType expected)
{
    Token temp = tokenVec[index];
    if(temp.token_type != expected)
    {
        syntax_error();
    }
    index++;
    return temp;
}

void Parser::parse_program()
{
    if(peek(1).token_type == ID)
    {
        parse_global_vars();
    }
    parse_body();
    expect(END_OF_FILE);
    Typer::print();
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
    vector<Variable*> varList = parse_var_list();
    expect(COLON);
    varType type = parse_type_name();
    Typer::set_var_types(varList, type);
    Typer::varList.insert(Typer::varList.end(), varList.begin(), varList.end());
    expect(SEMICOLON);
}

vector<Variable*> Parser::parse_var_list()
{
    vector<Variable*> varList;
    Token newTok = expect(ID);
    Variable* newVar = new Variable(newTok.lexeme, newTok.line_no, NONE_T);
    varList.push_back(newVar);
    if(peek(1).token_type == COMMA)
    {
        expect(COMMA);
        vector<Variable*> moreVars = parse_var_list();
        varList.insert(varList.end(), moreVars.begin(), moreVars.end());
    }
    return varList;
        
}

varType Parser::parse_type_name()
{
    if(peek(1).token_type == BOOL || peek(1).token_type == REAL || peek(1).token_type == INT)
    {
        Token token = expect(peek(1).token_type);
        if(token.token_type == BOOL)
            return BOOL_T;
        if(token.token_type == REAL)
            return REAL_T;
        return INT_T;
    }
    else
        syntax_error();
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
        case ID:
            parse_assignment_stmt();
            break;
        case IF:
            parse_if_stmt();
            break;

        case WHILE:
            parse_while_stmt();
            break;

        case SWITCH:
            parse_switch_stmt();
            break;

        default:
            syntax_error();
            break;
    }
}

void Parser::parse_assignment_stmt()
{
    Token lhs = expect(ID);
    if(Typer::searchVarList(lhs.lexeme) == -1)
    {
        Typer::varList.push_back(new Variable(lhs.lexeme, lhs.line_no, NONE_T));
    }
    expect(EQUAL);
    Expression* rhs = parse_expression();
    Variable* var;
    if(Typer::searchVarList(lhs.lexeme) == -1)
    {
        if(rhs->resolveType() == NONE_T)
        {
            vector<Variable*> newList;
            var = new Variable(lhs.lexeme, lhs.line_no, NONE_T);
            Typer::varList.push_back(var);
            newList.push_back(var);
            Typer::relateVars(rhs->getVars(), newList);
        }
        else
        {
            var = new Variable(lhs.lexeme, lhs.line_no, rhs->resolveType());
        }
    }
    else if(Typer::varList[Typer::searchVarList(lhs.lexeme)]->getType() == NONE_T)
    {
        if(rhs->resolveType() == NONE_T)
        {
            vector<Variable*> newList;
            
            newList.push_back(Typer::varList[Typer::searchVarList(lhs.lexeme)]);
            Typer::relateVars(rhs->getVars(), newList);
        }
        else
        {
            Typer::varList[Typer::searchVarList(lhs.lexeme)]->setType(rhs->resolveType());
        }
    }
    else
    {
        if(rhs->resolveType() == NONE_T)
        {
            Typer::set_var_types(rhs->getVars(), Typer::varList[Typer::searchVarList(lhs.lexeme)]->getType());
        }
        else
        {
            if(rhs->resolveType() != Typer::varList[Typer::searchVarList(lhs.lexeme)]->getType())
            {
                if(Typer::errorType == "")
                {
                    Typer::errorType = "C1";
                    Typer::error_line = lhs.line_no;
                }
            }
                //Typer::type_mismatch("C1", lhs.line_no);
        }
    }
    expect(SEMICOLON);
}

Expression* Parser::parse_expression()
{
    TokenType temp = peek(1).token_type;
    Expression* expr;

    if(temp == ID || temp == NUM || temp == REALNUM || temp == TRUE || temp == FALSE)
    {
        expr = new Primary_Expression(peek(1).line_no, nullptr);
        ((Primary_Expression*)expr)->setPrimary(parse_primary());
        return expr;
    }
    else if(temp == NOT)
    {
        expr = new Unary_Expression(peek(1).line_no, nullptr);
        parse_unary_operator();
        ((Unary_Expression*)expr)->setOp(parse_expression());
        return expr;
    }
    else
    {
        Token opSym = parse_binary_operator();
        expr = new Binary_Expression(peek(1).line_no, nullptr, opSym);
        
        ((Binary_Expression*)expr)->setOp1(parse_expression());
        ((Binary_Expression*)expr)->setOp2(parse_expression());
        return expr;
    }
}

void Parser::parse_unary_operator()
{
    expect(NOT);
}

Token Parser::parse_binary_operator()
{
    TokenType temp = peek(1).token_type;
    switch(temp)
    {
        case PLUS:
            return expect(PLUS);
            break;
        case MINUS:
            return expect(MINUS);
            break;
        case MULT:
            return expect(MULT);
            break;
        case DIV:
            return expect(DIV);
            break;
        case GREATER:
            return expect(GREATER);
            break;
        case LESS:
            return expect(LESS);
            break;
        case GTEQ:
            return expect(GTEQ);
            break;
        case LTEQ:
            return expect(LTEQ);
            break;
        case EQUAL:
            return expect(EQUAL);
            break;
        case NOTEQUAL:
            return expect(NOTEQUAL);
            break;
        default:
            syntax_error();
            break;
    }
}

Token Parser::parse_primary()
{
    TokenType temp = peek(1).token_type;
    if(temp == ID || temp == NUM || temp == REALNUM || temp == TRUE || temp == FALSE)
    {
        if(temp == ID)
        {
            if(Typer::searchVarList(peek(1).lexeme) == -1)
            {
                Variable* newVar = new Variable(peek(1).lexeme, peek(1).line_no, NONE_T);
                Typer::varList.push_back(newVar);
            }
        }
        return expect(temp);
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
    Expression* expr = parse_expression();
    if(expr->resolveType() != BOOL_T)
    {
        check* checker = new check;
        checker->expr = expr;
        checker->mismatch = "C4";
        checker->t = BOOL_T;
        Typer::exprToCheck.push_back(checker);
    }
        //Typer::type_mismatch("C4", expr->getLineNum());
    expect(RPAREN);
    parse_body();
}

void Parser::parse_while_stmt()
{
    expect(WHILE);
    expect(LPAREN);
    Expression* expr = parse_expression();
    if(expr->resolveType() != BOOL_T)
    {
        check* checker = new check;
        checker->expr = expr;
        checker->mismatch = "C4";
        checker->t = BOOL_T;
        Typer::exprToCheck.push_back(checker);
    }
        //Typer::type_mismatch("C4", expr->getLineNum());
    expect(RPAREN);
    parse_body();
}

void Parser::parse_switch_stmt()
{
    expect(SWITCH);
    expect(LPAREN);
    Expression* expr = parse_expression();
    if(expr->resolveType() != INT_T)
    {
        check* checker = new check;
        checker->expr = expr;
        checker->mismatch = "C5";
        checker->t = INT_T;
        Typer::exprToCheck.push_back(checker);
    }
        //Typer::type_mismatch("C5", expr->getLineNum());
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



