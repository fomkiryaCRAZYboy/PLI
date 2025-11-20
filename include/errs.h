#ifndef ERR_H
#define ERR_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_FAILS   20

/* Enum for all kinds of warnings */
typedef enum
{
    SUCCESS_w = 0,
    PLI_FREE_func_UNTRACKED_POINTER,
    WARNINGS_CODES_COUNT
} warnings;

/* Enum for all kinds of errors */
typedef enum
{
    SUCCESS = 0,
    MAIN_func_TOKENIZE_ERROR,
    CREATE_TOKEN_func_STREAM_OVERFLOW_ERROR,
    TOKENIZE_func_STREAM_CREATION_ERROR,
    TOKENIZE_func_GET_NUMBER_TOKEN_ERROR,
    TOKENIZE_func_GET_TEXT_TOKEN_ERROR,
    TOKENIZE_func_GET_SEP_OP_TOKEN_ERROR,
    TOKENIZE_func_GET_STR_TOKEN_ERROR,
    TOKENIZE_func_NUM_TOKEN_CREATION_ERROR,
    TOKENIZE_func_TEXT_TOKEN_CREATION_ERROR,
    TOKENIZE_func_SEP_OP_TOKEN_CREATION_ERROR,
    TOKENIZE_func_STRING_TOKEN_CREATION_ERROR,
    GET_NUMBER_TOKEN_func_ALLOC_ERROR,
    GET_NUMBER_TOKEN_func_BIG_NUMBER_ERROR,
    GET_NUMBER_TOKEN_func_INVALID_NUMBER,
    GET_TEXT_TOKEN_func_ALLOC_ERROR,
    GET_TEXT_TOKEN_func_INVALID_IDENTIFIER_ERROR,
    GET_TEXT_TOKEN_func_LONG_IDENTIFIER_ERROR,
    GET_TEXT_TOKEN_func_LONG_STRING_ERROR,
    GET_TEXT_TOKEN_func_QUOTE_NOT_FOUND_ERROR,
    GET_SEP_OP_TOKEN_func_ALLOC_ERROR,
    GET_SEP_OP_TOKEN_func_INVALID_SEP_OP_ERROR,
    ERRORS_CODES_COUNT
} f_result;

/*  the structure stores the error 
    code and the function that 
    terminated with this error */
typedef struct 
{
    short  err_code;
    short  line_num;
    bool   warning;
}
err_code ;

void add_err_code(int error_code, int line_num, bool warning);
void print_errors();
const char* decode_err(int error_code, bool warning);

#endif /* ERR_H */
