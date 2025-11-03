#ifndef LEXER_H
#define LEXER_H

#include "errs.h"

/* token text size for different types */
#define MAX_SEP_AND_OP_SIZE          3
#define MAX_BOOL_SIZE                7
#define MAX_FLOAT_SIZE               30      
#define MAX_INT_SIZE                 20       
#define MAX_TOKEN_TEXT_SIZE          64     /* Если максимальная длна текста токена 64,
                                             то необходимо сделать макс. длину имени переменной 
                                              и макс. длину для string 64 */

#define MAX_TOKENS_COUNT_IN_BLOCK    1024


/* token types for PLI lexer */
typedef enum 
{
    /* --- KEYWORDS (reserved words) --- */
    kw_token_if   ,     /* 'if'    - conditional statement */
    kw_token_else ,     /* 'else'  - alternative branch */
    kw_token_while,     /* 'while' - loop statement */
    kw_token_print,     /* 'print' - output function */
    kw_token_read ,     /* 'read'  - input function */
    kw_token_and  ,     /* 'and'   - logical operator */
    kw_token_or   ,     /* 'or'    - logical operator */
    kw_token_not  ,     /* 'not'   - logical operator */
    kw_token_end  ,     /* 'end'   - program termination */

    /* --- IDENTIFIERS --- */
    iden_token    ,      /* identifier - variable, e.g.: 'x', 'count', 'my_var' */

    /* --- LITERALS --- */
    num_token   ,        /* number literal, e.g. : '42', '3.14', '-10' */
    str_token   ,        /* string literal, e.g. : '"hello"', '"world"' */
    bool_token_t,        /* boolean true, e.g.   : 'true' */
    bool_token_f,        /* boolean false, e.g.  : 'false' */
    
    /* --- ARITHMETIC OPERATORS --- */
    math_op_token_mult ,   /* multiplication, e.g.: '*' */
    math_op_token_div  ,   /* division, e.g.      : '/' */
    math_op_token_plus ,   /* addition, e.g.      : '+' */
    math_op_token_minus,   /* subtraction, e.g.   : '-' */

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
    sep_token_semicolon,       /* semicolon, e.g.         : ';' */ 
    sep_token_dot      ,       /* dot, e.g.               : '.' */


    unknown_token              /* unknown/error token */
} 
TOKEN_TYPE ;


/* Token text union for different data types */
typedef union token_value
{
    char text            [MAX_TOKEN_TEXT_SIZE];     /* For identifiers, strings, keywords */
    char int_text        [MAX_INT_SIZE];            /* For integer numbers */
    char float_text      [MAX_FLOAT_SIZE];          /* For floating point numbers */
    char bool_text       [MAX_BOOL_SIZE];           /* For boolean values */
    char sep_and_op_text [MAX_SEP_AND_OP_SIZE];     /* For separators and math/comp operations */
} 
TOKEN_TEXT ;

/*  */
typedef struct token
{
    TOKEN_TEXT value_token ;
    TOKEN_TYPE type_token  ;
} 
TOKEN ;

/* (block of code) */
f_result tokenize(char* block) ;
f_result create_token(TOKEN_TYPE token_type, char* token_text, TOKEN* tokens_stream, int* tokens_count_in_stream) ;

#endif /* LEXER_H */