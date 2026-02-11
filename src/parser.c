#include "errs.h"
#include "lexer.h"
#include "mem.h"
#include "parser_api.h"
#include "parser.h"

#include <stdbool.h>
#include <stdio.h>

#if 0
kw_token_var  ,     /* 'var'   - variable declare keyword */
    kw_token_if   ,     /* 'if'    - conditional statement */
    kw_token_else ,     /* 'else'  - alternative branch */
    kw_token_while,     /* 'while' - loop statement */
    kw_token_print,     /* 'print' - output function */
    kw_token_read ,     /* 'read'  - input function */
    kw_token_and  ,     /* 'and'   - logical operator */
    kw_token_or   ,     /* 'or'    - logical operator */
    kw_token_not  ,     /* 'not'   - logical operator */

    /* --- IDENTIFIERS --- */
    iden_token,          /* identifier - variable, e.g.: 'x', 'count', 'my_var' */

    /* --- LITERALS --- */
    str_token   ,        /* string literal, e.g. : '"hello"', '"world"' */
    num_token   ,        /* number literal, e.g. : '42', '3.14', '10' */
    bool_token_t,        /* boolean true, e.g.   : 'true' */
    bool_token_f,        /* boolean false, e.g.  : 'false' */
    
    /* --- ARITHMETIC OPERATORS --- */
    math_op_token_mult ,   /* multiplication, e.g.: '*' */
    math_op_token_div  ,   /* division, e.g.      : '/' */
    math_op_token_plus ,   /* addition, e.g.      : '+' */
    math_op_token_minus,   /* subtraction, e.g.   : '-' */
    math_op_token_percent,   /* remainder from division, e.g.   : '%' */


    /* --- ASSIGNMENT OPERATOR --- */
    assing_op_token,     /* assignment, e.g.      : '=' */

    /* --- COMPARISON OPERATORS --- */
    comp_op_token_equal     ,       /* equality, e.g.         : '==' */
    comp_op_token_not_equal ,       /* inequality, e.g.       : '!=' */
    comp_op_token_more      ,       /* greater than, e.g.     : '>' */
    comp_op_token_less      ,       /* less than, e.g.        : '<' */
    comp_op_token_less_equal,       /* less or equal, e.g.    : '<=' */
    comp_op_token_more_equal,       /* greater or equal, e.g. : '>=' */

    /* --- SEPARATORS --- */
    sep_token_lparen   ,       /* left parenthesis, e.g.  : '(' */
    sep_token_rparen   ,       /* right parenthesis, e.g. : ')' */
    sep_token_lbrace   ,       /* left brace, e.g.        : '{' */
    sep_token_rbrace   ,       /* right brace, e.g.       : '}' */
#if 0
    /* no implementation yet */

    sep_token_lbracket ,       /* left bracket, e.g.      : '[' */
    sep_token_rbracket ,       /* right bracket, e.g.     : ']' */
#endif    
    sep_token_comma    ,       /* comma, e.g.             : ',' */
    sep_token_dot      ,       /* dot, e.g.               : '.' */


    unknown_token              /* unknown/error token */
#endif

/* add new stmt to stmts_list */
static void add_stmt_to_list(stmt_node_t* stmt, program_t* program)
{   
    if(program -> statements == NULL)
        /* first statement in list */
        program -> statements = stmt;
    else
    {
        /* find the last stmt in list */
        stmt_node_t* curr = program -> statements;
        while(!curr -> next)
            curr = curr -> next;

        curr -> next = stmt;
    }

    program -> stmt_count++;
}

/*  */
static stmt_node_t* create_stmt(void* any_stmt, stmt_type_t type, int line_num)
{
    stmt_node_t* stmt = pli_alloc(sizeof(stmt_node_t));
    if(!stmt)
    {
        add_err_code(MEMORY_ALLOCATION_ERROR, -1, false);
        return NULL;
    }

    stmt -> type = type;

    switch (type) 
    {
        case STMT_VAR_DECL:
            stmt -> as.var_decl = (var_decl_stmt_t*)any_stmt; 
            break;
        case STMT_ASSIGNMENT:
            stmt -> as.assignment = (assignment_stmt_t*)any_stmt;
            break;
        case STMT_BLOCK:
            stmt -> as.block = (block_stmt_t*)any_stmt;
            break;
        case STMT_IF:
            stmt -> as.if_stmt = (if_stmt_t*)any_stmt;
            break;
        case STMT_PRINT:
            stmt -> as.print_stmt = (print_stmt_t*)any_stmt;
            break;
        case STMT_READ:
            stmt -> as.read_stmt = (read_stmt_t*)any_stmt;
            break;
        case STMT_WHILE:
            stmt -> as.while_stmt = (while_stmt_t*)any_stmt;
            break;
        
        default: 
            goto err;    
    }

    stmt -> next = NULL;
    return stmt;

err:
pli_free(stmt);
add_err_code(CREATE_STMT_func_UNKNOWN_STMT_TYPE, line_num, false);
return NULL;
}

/* main parse function. it has one parameter - tokens stream returned by tokenize() func. */
program_t* parse(TOKEN_STREAM* stream)
{
    program_t* program = pli_alloc(sizeof(program_t));
    if(!program)
    {
        add_err_code(MEMORY_ALLOCATION_ERROR, -1, false);
        return NULL;
    }

    program -> stmt_count = 0;
    program -> statements = NULL;

    for (int idx = 0; idx < stream->count; ++idx)
    {
        switch (stream ->tokens[idx].type_token)
        {
            case kw_token_var:
            {
                var_decl_stmt_t* var_decl_stmt = create_var_decl_stmt(stream, idx);
                if(!var_decl_stmt)
                {
                    add_err_code(PARSE_func_CRAETE_VAR_DECL_STMT_ERROR, stream -> tokens[idx].line_number, false);
                    return NULL;
                }
                stmt_node_t* stmt = create_stmt(var_decl_stmt, STMT_VAR_DECL, stream -> tokens[idx].line_number);
                if(!stmt)
                {
                    add_err_code(PARSE_func_STMT_CREATION_ERROR, stream -> tokens[idx].line_number, false);
                    return NULL;
                }

                add_stmt_to_list(stmt, program);
                break;
            }
            default: 
                return NULL;
        }
    }

    return program;
}