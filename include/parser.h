#ifndef PARSER_H
#define PARSER_H

#define BOOL_TRUE  1
#define BOOL_FALSE 0

#include "lexer.h"

#define MAX_STR_SIZE    MAX_TOKEN_TEXT_SIZE
#define MAX_VAR_SIZE    MAX_TOKEN_TEXT_SIZE

/* 
предположительно, функция parsing() должна возвращать 
список AST-узлов (то есть AST-дерево), предназначенных для дальнейшего исполнения 

AST-узел - структура данных, обозначающая конкретную атомарную операцию, к примеру:
 - "5 + 2"
 - "print(x)"
 - "var x = 1"

Операция "var tmp = 10 - 3" не является атомарной и будет разбита на:
1) "10 - 3"  -->  7
2) "var tmp = 7"

То есть, var tmp равен результату исполнения предыдущего узла.
По этому принципу должно строится абстрактное синтаксическое дерево.
*/

/* Forward declaration */
typedef struct ast_node AST_BASE;

typedef enum node_type
{
    VAR_DECLARATION_NODE,     /* var x = "str"; */

    /* statements */
    IF_STATEMENT_NODE,        /* if(...) {...} */
    WHILE_STATEMENT_NODE,     /* while(...) {...} */
    PRINT_STATEMENT_NODE,     /* print(...) */
    READ_STATEMENT_NODE,      /* read(...) */

    CONDITION_NODE,           /* (x > 5 and y != "hello") */
    BODY_NODE,                /* { ... } */  /*example: {read(x); print(x); end;} */

    LITERAL_NODE,             /* 5, "str", 1, 0.4 */
    VARIABLE_NODE,            /* var_x, y */

    /* simple operations */

    /* assignment operation */
    ASSIGNMENT_OP_NODE,       /* y = 12.4; */

    /* all boolean operations */
    BINARY_OP_NODE,           /* x > 3 */ 
                              /* var_x < 23 */ 
                              /* tmp >= 0 */
                              /* z <= 0 */
                              /* x == 0 */
                              /* x != 0 */

    /*  all mathematic operations */
    MATH_OP_NODE,             /* x * 4.3 */
                              /* 5 / 2 */
                              /* 113 + 34.45 */
                              /* 80 - 3 */

    END_NODE                  /* end; */ /* termination program */
}
NODE_TYPE ;

/* base AST node structure */
struct ast_node
{
    NODE_TYPE type;  /* type of node */
    AST_BASE* next;  /* pointer to the node that will be executed next */
};

/* data types for literals and variables */
typedef enum data_type { INT_TYPE, FLOAT_TYPE, BOOL_TYPE, STR_TYPE, NONE_TYPE } DATA_TYPE ;
/* NONE_TYPE for newly declared variables: "var x;" */


/* -----Specific Nodes----- */

/* literal AST node */
typedef struct 
{
    AST_BASE base;

    DATA_TYPE data_type;
    union 
    {
        int int_data ;
        float float_data ;
        int bool_data ;
        char str_data[MAX_STR_SIZE];
    };
}
LITERAL_NODE ;

typedef struct 
{
    AST_BASE base;

    char variable_name[MAX_VAR_SIZE];
    DATA_TYPE data_type;      /* NONE_TYPE for newly declared variables */
    union 
    {
        int int_data ;
        float float_data ;
        int bool_data ;
        char str_data[MAX_STR_SIZE];
    };
} 
VARIABLE_NODE ;




#endif /* PARSER_H */
