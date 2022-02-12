#ifndef _PARSE_H_
#define _PARSE_H_
#include "typer.h"

class Parser
{
    public:
        Parser();
        void parse_program();
        void pushToVec(Token t);
        Token peek(int dist);
        void parse_global_vars();
        std::vector<Variable*> parse_var_list();
        void parse_var_decl_list();
        void parse_var_decl();
        varType parse_type_name();
        void parse_body();
        void parse_assignment_stmt();
        Expression* parse_expression();
        void parse_unary_operator();
        Token parse_binary_operator();
        Token parse_primary();
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
        std::vector<Token> tokenVec;
        int index;


};


#endif
