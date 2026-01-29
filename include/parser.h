#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "var.h"
#include "ops.h"

#define MAX_PRINT_ARGS  24

typedef enum
{
    /* --- STATEMENTS --- */
    IF_STATEMENT,       /* if (...) {...} */ 
                        /* 
                            if_statement consists of 
                            'if' keyword,
                            body_node and condition_node 
                        */

    WHILE_STATEMENT,    /* while (...) {...} */ 
                        /* 
                            while_statement consists of 
                            'while' keyword,
                            body_node and condition_node 
                        */

    READ_STATEMENT,     /* read (<var_name>); */
                        /*
                            read_statement consists of
                            'read' keyword and variable (variable_t)
                        */

    PRINT_STATEMENT,    /* print (expression | value | variable); */
                        /*
                            print_statement consists of
                            'print' keyword, expression (expression_t),
                            value (value_t) or variable (variable_t)
                        */

    BODY,               /* {...} */
                        /* body_node can consist of an entire subroutine */

    CONDITION,          /* (<expression>) */ 
                        /* 
                            condition for 'if'-branching or 'while'-looping
                            consists of expression (expression_t)  
                        */

    VAR_DECLARATION,    /* var <var_name>; */ /* var <var_name> = <expression>; */
    ASSIGNMENT_OP       /* <var_name> = <expression>; */
}
NODE_TYPE ;

        /* --- BASE NODE ---*/
typedef struct ast_node
{
    NODE_TYPE   node_type;    /* specific node type */
    void*       spec_node;    /* pointer to a specific node */  

    struct ast_node_t* next_node;    /* pointer to a BASE NODE that will be executed next */ 
}
ast_node_t ;

        /* --- SPECIFIC NODES --- */

        /* --- VARIABLE OPERATION node --- */
/* variable declaration/assignment operation node */
typedef struct var_operation
{
    variable_t*   variable;      /* pointer to variable */
    expression_t* assign_expr;   /* 
                                    if "var x;", assign_expr leads to NULL.
                                    assign_expr can not be NULL in assignment operation 
                                */
}
var_operation_t ;

        /* --- CONDITION node --- */
typedef struct condition
{
    expression_t* cond_expr;    /* eventually the condition expression will turn into a value_t -> true or false*/
}
condition_t ;

        /* --- BODY node --- */
/* subroutine tokens will be recursively passed to the parsing function */
typedef struct body
{
    TOKEN* body_tokens;
}
body_t ;

        /* --- PRINT STATEMENT node --- */
typedef struct print_st
{
    expression_t* print_exprs;  /* pointer to the expression(s) to be printed */
    int expr_count;
}
print_st_t ;

        /* --- READ STATEMENT node --- */
typedef struct read_st
{
    variable_t* var;  /* pointer to the variable into which the value will be read */
}
read_st_t ;

        /* --- IF/WHILE STATEMENT node --- */
typedef struct if_whwile_statement
{
    condition_t* cond;   /* pointer to condition */
    body_t* body;        /* pointer to body */
}
if_while_statement_t;

#endif /* PARSER_H */