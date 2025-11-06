#ifndef LEXER_H
#define LEXER_H

#include "errs.h"
#include <stddef.h>

/* token text size for different types */
#define MAX_SEP_AND_OP_SIZE          3
#define MAX_BOOL_SIZE                6
#define MAX_NUM_SIZE                 30      
#define MAX_TOKEN_TEXT_SIZE          64     /* Если максимальная длна текста токена 64,
                                             то необходимо сделать макс. длину имени переменной 
                                              и макс. длину для string 64 */

#define MAX_TOKENS_COUNT_IN_BLOCK    1024


/* token types for PLI lexer */
typedef enum 
{
    /* --- KEYWORDS (reserved words) --- */
    kw_token_var  ,     /* 'var'   - variable declare keyword */
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
    str_token   ,        /* string literal, e.g. : '"hello"', '"world"' */
    num_token   ,        /* number literal, e.g. : '42', '3.14', '-10' */
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
    char num_text        [MAX_NUM_SIZE];            /* For integer and float numbers */
    char bool_text       [MAX_BOOL_SIZE];           /* For boolean values */
    char sep_and_op_text [MAX_SEP_AND_OP_SIZE];     /* For separators and math/comp operations */
} 
TOKEN_TEXT ;

/* Token structure */
typedef struct token
{
    TOKEN_TEXT text_token ;
    TOKEN_TYPE type_token  ;
    int line_number ;        /* Line number where token was found (1-based) */
} 
TOKEN ;

/* Token stream structure - encapsulates token array with metadata */
typedef struct token_stream
{
    TOKEN* tokens ;           /* Array of tokens */
    size_t count ;            /* Number of tokens in stream */
    int current_line ;        /* Current line number being processed */
} 
TOKEN_STREAM ;

/* (block of code) */
TOKEN_STREAM* tokenize(char* block) ;

f_result create_token(TOKEN_TYPE token_type, char* token_text, TOKEN_STREAM* stream, int line_number) ;
char* get_number_token(char** line_ptr, int line_number);

/* debug */
void debug_print_stream(TOKEN_STREAM* stream);
const char* get_type_of_token(TOKEN_TYPE type);

#endif /* LEXER_H */