#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "var.h"

#include <stdbool.h>

/* Maximum number of arguments in print() */
#define MAX_PRINT_ARGS  24

/* Forward declarations */
typedef struct expr_node expr_node_t;
typedef struct stmt_node stmt_node_t;

/* ============================================================================
 * EXPRESSION AST NODES
 * ============================================================================ */

/* Expression node types */
typedef enum 
{
    EXPR_BINARY,       /* Binary operation: <left> <op> <right> */
    EXPR_UNARY,        /* Unary operation: <op> <operand> */
    EXPR_LITERAL,      /* Literal value: number, string, boolean */
    EXPR_VARIABLE,     /* Variable reference */
    EXPR_GROUPING      /* Grouped expression: (expr) */
} expr_type_t;

/* Binary operators */
typedef enum 
{
    /* Logical operators */
    OP_AND,            /* and */
    OP_OR,             /* or */
    
    /* Comparison operators */
    OP_EQUAL,          /* == */
    OP_NOT_EQUAL,      /* != */
    OP_LESS,           /* < */
    OP_GREATER,        /* > */
    OP_LESS_EQUAL,     /* <= */
    OP_GREATER_EQUAL,  /* >= */
    
    /* Arithmetic operators */
    OP_ADD,            /* + */
    OP_SUBTRACT,       /* - */
    OP_MULTIPLY,       /* * */
    OP_DIVIDE,         /* / */
    OP_MODULO          /* % */
} binary_op_t;

/* Unary operators */
typedef enum {
    OP_NEGATE,         /* - (arithmetic negation) */
    OP_NOT             /* not (logical negation) */
} unary_op_t;

/* Literal value types */
typedef enum {
    LIT_NUMBER,        /* Integer or float */
    LIT_STRING,        /* String literal */
    LIT_BOOLEAN        /* true or false */
} literal_type_t;

/* Literal value union */
typedef union {
    double number;                      /* For both int and float */
    char string[MAX_STR_SIZE];   /* String value */
    bool boolean;                       /* Boolean value */
} literal_value_t;

/* Binary expression node */
typedef struct {
    binary_op_t  op;
    expr_node_t* left;
    expr_node_t* right;
} binary_expr_t;

/* Unary expression node */
typedef struct {
    unary_op_t   op;
    expr_node_t* operand;
} unary_expr_t;

/* Literal expression node */
typedef struct {
    literal_type_t  type;
    literal_value_t value;
} literal_expr_t;

/* Variable reference node */
typedef struct {
    char name[MAX_VAR_SIZE];
} variable_expr_t;

/* Grouping expression node */
typedef struct {
    expr_node_t* expression;
} grouping_expr_t;

/* Main expression node (tagged union) */
struct expr_node {
    expr_type_t type;
    union {
        binary_expr_t*   binary;
        unary_expr_t*    unary;
        literal_expr_t*  literal;
        variable_expr_t* variable;
        grouping_expr_t* grouping;
    } expr;
};

/* ============================================================================
 * STATEMENT AST NODES
 * ============================================================================ */

/* Statement node types */
typedef enum {
    STMT_VAR_DECL,     /* var x = expr */
    STMT_ASSIGNMENT,   /* x = expr */
    STMT_IF,           /* if (expr) { stmts } [else { stmts }] */
    STMT_WHILE,        /* while (expr) { stmts } */
    STMT_PRINT,        /* print(expr, expr, ...) */
    STMT_READ,         /* read(x) */
    STMT_BLOCK         /* { stmts } */
} stmt_type_t;

/* Variable declaration statement */
typedef struct {
    char var_name[MAX_VAR_SIZE];
    expr_node_t* initializer;  /* Expression to initialize variable */
} var_decl_stmt_t;

/* Assignment statement */
typedef struct {
    char var_name[MAX_VAR_SIZE];
    expr_node_t* value;  /* Expression to assign */
} assignment_stmt_t;

/* If statement */
typedef struct {
    expr_node_t* condition;
    stmt_node_t* then_branch;      /* Block or single statement */
    stmt_node_t* else_branch;      /* NULL if no else clause */
} if_stmt_t;

/* While statement */
typedef struct {
    expr_node_t* condition;
    stmt_node_t* body;  /* Block or single statement */
} while_stmt_t;

/* Print statement */
typedef struct {
    expr_node_t** expressions;  /* Array of expressions to print */
    int expr_count;             /* Number of expressions */
} print_stmt_t;

/* Read statement */
typedef struct {
    char var_name[MAX_VAR_SIZE];
} read_stmt_t;

/* Block statement */
typedef struct {
    stmt_node_t** statements;  /* Array of statements in block */
    int stmt_count;            /* Number of statements */
} block_stmt_t;

/* Main statement node (tagged union) */
struct stmt_node {
    stmt_type_t type;
    union {
        var_decl_stmt_t* var_decl;
        assignment_stmt_t* assignment;
        if_stmt_t* if_stmt;
        while_stmt_t* while_stmt;
        print_stmt_t* print_stmt;
        read_stmt_t* read_stmt;
        block_stmt_t* block;
    } as;
    
    stmt_node_t* next;  /* For statement sequences (linked list) */
};

/* ============================================================================
 * PROGRAM (root of AST)
 * ============================================================================ */

/* Program node - root of AST tree */
typedef struct {
    stmt_node_t* statements;   /* ptr to first stmt in linked list of statements */
    int stmt_count;            /* Number of statements */
} program_t;


/* DEBUG */
void print_ast(program_t* program);

#endif /* PARSER_H */