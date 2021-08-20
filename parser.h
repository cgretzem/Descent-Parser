#ifndef _PARSE_H_
#define _PARSE_H_
#include "type_checker.h"

class Parser
{
    public:
        Parser();
        void parse_program();
        void pushToVec(Token t);
        Token peek(int dist);
        void parse_global_vars();
        void parse_var_list();
        void parse_var_decl_list();
        void parse_var_decl();
        void parse_type_name();
        void parse_body();
        void parse_assignment_stmt();
        void parse_expression();
        void parse_unary_operator();
        void parse_binary_operator();
        void parse_primary();
        void parse_if_stmt();
        void parse_while_stmt();
        void parse_switch_stmt();
        void parse_case_list();
        void parse_case();
        void parse_statement_list();
        void parse_statement();
        Token expect(TokenType expected);
        void syntax_error();
        Typer typer;
    private:
        InputBuffer input;
        LexicalAnalyzer lex;
        vector<Token> tokenVec;
        int index;


};


#endif
