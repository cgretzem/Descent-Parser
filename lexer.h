
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    PUBLIC, PRIVATE,
    INT,REAL,BOOL,TRUE,FALSE,IF,
    WHILE,SWITCH,CASE,NOT,PLUS,MINUS,MULT,
    DIV,GREATER,LESS,GTEQ,LTEQ,
    NOTEQUAL,LPAREN,RPAREN,NUM,
    REALNUM, 
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRACE, RBRACE, ID, ERROR 
} TokenType;

//Represents a token from inputBuf
class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

//LexicalAnalyzer takes in tokens before parsing and typing starts and puts them in a vector of tokens
class LexicalAnalyzer {
  public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipComment();
    bool SkipSpace();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNum();

};

#endif  //__LEXER__H__
