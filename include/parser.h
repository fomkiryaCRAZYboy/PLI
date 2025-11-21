#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "var.h"
#include "ops.h"

#define MAX_PRINT_ARGS  24

/*
необходимо создать структуру общего узла base_node_t, в которой будет указатель на base_node_t, который нужно исполнить следующим
в этой стркутуре будет void* ptr на узел конкретного типа.
*/

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

/* node is an atomic operation */
typedef enum
{
    /* variable declaration */
    VAR_DECLARATION_NODE,     /* var x = "str"; */  /* var x; */
    /* assignment operation */
    ASSIGNMENT_OP_NODE,       /* y = 12.4; */

    /*  all mathematic operations */
    MATH_OP_NODE,             /* x * 4.3 */
                              /* 5 / 2 */
                              /* 113 + 34.45 */
                              /* 80 - 3 */

    /* all boolean operations */
    BINARY_OP_NODE,           /* x > 3 */ 
                              /* var_x < 23 */ 
                              /* tmp >= 0 */
                              /* z <= 0 */
                              /* x == 0 */
                              /* x != 0 */

    /* all logical operations */
    LOGICAL_OP_NODE,           /* and, or, not */

    /* read and print statements */
    IO_OP_NODE                /* read, print */
}
NODE_TYPE ;

/* base node structure */
typedef struct
{
    NODE_TYPE    node_type;     /* specific node type */
    void*        specific_node; /* pointer to specific node */
}
base_node_t;


/* ---specific nodes--- */

/* for declaration and assignment */
typedef struct
{
    variable_t* var;         /* declared variable */
    value_t*    value;       /* assigned value */
}
var_operation_unit_t;

/* for addition, subtraction, division and multiplication ops */
typedef struct
{
    MATH_OP  op;    /* +, -, *, / */
    value_t* left_operand;
    value_t* right_operand;
}
math_operation_unit_t;

/* for compare ops */
typedef struct
{
    BIN_OP  op;    /* =, !=, >, <, >=, <= */
    value_t* left_operand;
    value_t* right_operand;
}
bin_operation_unit_t;

/* for logical ops */
typedef struct
{
    LOGICAL_OP  op;    /* and, or, not */
    value_t     left_operand;
    value_t     right_operand;
}
logical_operation_unit_t;

/* for read() and print() statements (are treated as atomic operations) */
typedef struct
{
    IO_OP    op;            /* READ, PRINT */
    value_t* values_array;  /* !!!only one value for read */
}
io_operation_unit_t;


#endif /* PARSER_H */