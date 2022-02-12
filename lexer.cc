/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */

#include <istream>
#include <vector>
#include <string>
#include <cctype>


#include "parser.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "PUBLIC", "PRIVATE","INT", "REAL", 
    "BOOl", "TRUE","FALSE", "IF", 
    "WHILE", "SWITCH","CASE",  
    "GTEQ", "LTEQ", "NOTEQUAL", "NOT", "PLUS", "MINUS",
    "MULT", "DIV", "GREATER", "LESS","LPAREN",
    "RPAREN", "NUM", "REALNUM",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE", "ID", "ERROR" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 11
string keyword[] = { "public", "private", "int", "real", "bool", "true", "false", "if", "while", "switch", "case"};

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}


bool LexicalAnalyzer::SkipComment()
{
    char c;
    bool comment_encountered = false;

    input.GetChar(c);

    line_no+= (c == '\n');

    if(c == '/')
    {
        
        input.GetChar(c);
        if(c == '/')
        {
            input.UngetChar(c);
            while(!input.EndOfInput() && c != '\n')
            {
                comment_encountered = true;
                input.GetChar(c);
                line_no += (c == '\n');
            }

            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }
    }
    else
    {
        input.UngetChar(c);
    }
    return comment_encountered;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

int countCapitals(string boof)
{
    int count = 0;
    for(int i = 0; i < boof.length(); i++)
    {
        if(isupper(boof[i]))
        {
            count++;
        }
    }
    return count;
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
        {
            if(countCapitals(tmp.lexeme) > 1)
            {
                tmp.lexeme = "";
                tmp.token_type = ERROR;
            }
            else
                tmp.token_type = ID;
        }
            
    }
    else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

Token LexicalAnalyzer::ScanNum()
{
    char c;
    input.GetChar(c);
    if(isdigit(c) && c != '0')
    {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isdigit(c))
        {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) 
        {
            input.UngetChar(c);
        }
    }
    else
    {
        tmp.lexeme = "0";
    }

    input.GetChar(c);
    if(c == '.')
    {
        tmp.lexeme += ".";
        input.GetChar(c);
        if(isdigit(c))
        {
            while(!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) 
            {
                input.UngetChar(c);
            }
            tmp.token_type = REALNUM;
            tmp.line_no = line_no;
        }
        else
        {
            tmp.token_type = ERROR;
            tmp.line_no = line_no;
        }
    }
    else
    {
        input.UngetChar(c);
        tmp.token_type = NUM;
        tmp.line_no = line_no;
    }

   return tmp;
}

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    //SkipComment();

    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        case '!':
            tmp.token_type = NOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '>':
            input.GetChar(c);
            if(c == '=')
            {
                tmp.token_type = GTEQ;
            }
            else{
                input.UngetChar(c);
                tmp.token_type = GREATER;
            }
            
            return tmp;
        case '<':
            input.GetChar(c);
            if(c == '=')
            {
                tmp.token_type = LTEQ;
            }
            else if (c == '>')
            {
                tmp.token_type = NOTEQUAL;
            }
            else{
                input.UngetChar(c);
                tmp.token_type = LESS;
            }
            
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
       
        default:
           if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            }
            else if (isdigit(c))
            {
                input.UngetChar(c);
                return ScanNum();
            }
            else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;
    InputBuffer input;
    Parser parser;
    
    token = lexer.GetToken(); // this block is outside while to initialize token

    if(token.token_type != ERROR)
    {
        parser.pushToVec(token);
    }
    
    //adding all non-error tokens to vector
    while(token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();

        if(token.token_type != ERROR)
        {
            parser.pushToVec(token);
        }
       
    }
    //parsing program
    parser.parse_program();



}
