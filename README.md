       
```
 _____  ______ _____ _    _ _____   _____ _______      ________ 
 |  __ \|  ____/ ____| |  | |  __ \ / ____|_   _\ \    / /  ____|
 | |__) | |__ | |    | |  | | |__) | (___   | |  \ \  / /| |__   
 |  _  /|  __|| |    | |  | |  _  / \___ \  | |   \ \/ / |  __|  
 | | \ \| |___| |____| |__| | | \ \ ____) |_| |_   \  /  | |____ 
 |_|  \_\______\_____|\____/|_|  \_\_____/|_____|   \/   |______|
  _____  ______  _____  _____ ______ _   _ _______ 
 |  __ \|  ____|/ ____|/ ____|  ____| \ | |__   __|
 | |  | | |__  | (___ | |    | |__  |  \| |  | |   
 | |  | |  __|  \___ \| |    |  __| | . ` |  | |   
 | |__| | |____ ____) | |____| |____| |\  |  | |   
 |_____/|______|_____/ \_____|______|_| \_|  |_|   
  _____        _____   _____ ______ _____  
 |  __ \ /\   |  __ \ / ____|  ____|  __ \ 
 | |__) /  \  | |__) | (___ | |__  | |__) |
 |  ___/ /\ \ |  _  / \___ \|  __| |  _  / 
 | |  / ____ \| | \ \ ____) | |____| | \ \ 
 |_| /_/    \_\_|  \_\_____/|______|_|  \_\
                                           
                                           
                                                                                                                                                               
 ```


A Recursive Descent Parser is a top-down parser built from a set of mutually recursive procedures where each such procedure implements one of the nonterminals of the grammar. 


This project's input coding langage consists of 3 main parts 

**1. Lexical Requirements** - these are the actual keywords that are allowed to be used in the coding langage. Anything that the lexer decides is not within these lexical
    specifications raise an error.
    
    
    INT = "int"
    REAL = "real"
    BOOL = "bool"
    TRUE = "true"
    FALSE = "false"
    IF = "if"
    WHILE = "while"
    SWITCH = "switch"
    CASE = "case"
    NOT = "!"
    PLUS = "+"
    MINUS = "-"
    MULT = "*"
    DIV = "/"
    GREATER = ">"
    LESS = "<"
    GTEQ = ">="
    LTEQ = "<="
    NOTEQUAL = "<>"
    LPAREN = "("
    RPAREN = ")"
    NUM = (pdigit digit*) + 0
    REALNUM = NUM "." digit digit*
    PUBLIC = "public"
    PRIVATE = "private"
    EQUAL = "="
    COLON = ":"
    COMMA = ","
    SEMICOLON = ";"
    LBRACE = "{"
    RBRACE = "}"
    ID = letter (letter + digit)*
    
    
**2. Grammar** - the input code must follow this grammar, otherwise an error is thrown. It is designed so a recursive descent parser may be implemented to parse it.
    
    ---------------------------------------------------------------
    program -> global_vars body
    global_vars -> ε
    global_vars -> var_decl_list
    var_decl_list -> var_decl
    var_decl_list -> var_decl var_decl_list
    var_decl -> var_list COLON type_name SEMICOLON
    var_list -> ID
    var_list -> ID COMMA var_list
    type_name -> INT
    type_name -> REAL
    type_name -> BOOL
    body -> LBRACE stmt_list RBRACE
    stmt_list -> stmt
    stmt_list -> stmt stmt_list
    stmt -> assignment_stmt
    stmt -> if_stmt
    stmt -> while_stmt
    stmt -> switch_stmt
    assignment_stmt -> ID EQUAL expression SEMICOLON
    expression -> primary
    expression -> binary_operator expression expression
    expression -> unary_operator expression
    unary_operator -> NOT
    binary_operator -> PLUS | MINUS | MULT | DIV
    binary_operator -> GREATER | LESS | GTEQ | LTEQ | EQUAL | NOTEQUAL
    primary -> ID
    primary -> NUM
    primary -> REALNUM
    primary -> TRUE
    primary -> FALSE
    if_stmt -> IF LPAREN expression RPAREN body
    while_stmt -> WHILE LPAREN expression RPAREN body
    switch_stmt -> SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE
    case_list -> case
    case_list -> case case_list
    case -> CASE NUM COLON body

    
**3. Semantics** - these are the rules that make sure the program makes sense, for example checking to make sure the condition of an if statment is bool-type.

    
      3.1. Types
        The language has three built-in types: int , real and bool .
        
      3.2. Variables
        Programmers can declare variables either explicitly or implicitly.
        Explicit variables are declared in an var_list of a var_decl
        
        A variable is declared implicitly if it is not declared explicitly but it appears in the
        program body
        
      3.3. Type System
        Our language uses structural equivalence for checking type equivalence. Implicit types will
        be inferred from the usage (in a simplified form of Hindley-Milner type inference).
        Here are all the type rules/constraints that my type checker will enforce (constraints are
        labeled for reference):
        
        *C1: The left hand side of an assignment should have the same type as its right hand side
        
        *C2: The operands of a binary operator ( GTEQ, LTEQ , EQUAL and NOTEQUAL
        PLUS , MINUS , MULT , DIV , GREATER , LESS)
        should have the same type (it can be any type)
        
        *C3: The operand of a unary operator ( NOT ) should be of type bool
        
        *C4: Condition of if and while statements should be of type bool
        
        *C5: The expression that follows the switch keyword in switch_stmt should be of type
        int
        
        *The type of expression binary_operator op1 op2 is the same as the type of op1 and
        op2 if operator is PLUS , MINUS , MULT or DIV . Note that op1 and op2 must have
        the same type due to C2
        
        *The type of expression binary_operator op1 op2 is bool if operator is GREATER ,
        LESS , GTEQ , LTEQ , EQUAL or NOTEQUAL
        
        *The type of expression unary_operator op is bool
        
        *NUM constants are of type int
        
        *REALNUM constants are of type real
        
        *true and false values are of type bool
**4. Output** - these is how the program is supposed to represet variables in a given input, or throw a type error if it finds one.
       
       
       There are two scenarios:
             * There is a type error in the input program
             * There are no type errors in the input program
             
       Type errors follow this format: 
   ```
   TYPE MISMATCH <line_number> <constraint>
   ```
