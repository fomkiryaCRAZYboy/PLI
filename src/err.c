#include "errs.h"

#include <stdbool.h>
#include <stdio.h>

static int errors_count = 0;
err_code errors_array[MAX_FAILS];

/* Array of warning messages, indexed by warnings enum */
static const char* warning_messages[] = 
{
    [SUCCESS_w] = "Success",
    [PLI_FREE_func_UNTRACKED_POINTER] = "TryingToFreeUntrackedPointer",
};

/* Array of error messages, indexed by f_result enum */
static const char* error_messages[] = 
{
    [SUCCESS] = "Success",
    [MAIN_func_TOKENIZE_ERROR] = "TokenizeError",
    [CREATE_TOKEN_func_STREAM_OVERFLOW_ERROR] = "TokenStreamOverflowError",
    [TOKENIZE_func_STREAM_CREATION_ERROR] = "StreamCreationError",
    [TOKENIZE_func_GET_NUMBER_TOKEN_ERROR] = "GetNumberTokenError",
    [TOKENIZE_func_GET_TEXT_TOKEN_ERROR] = "GetTextTokenError",
    [TOKENIZE_func_GET_SEP_OP_TOKEN_ERROR] = "GetSepOpTokenError",
    [TOKENIZE_func_GET_STR_TOKEN_ERROR] = "GetStringTokenError",
    [TOKENIZE_func_NUM_TOKEN_CREATION_ERROR] = "NumberTokenCreationError",
    [TOKENIZE_func_TEXT_TOKEN_CREATION_ERROR] = "TextTokenCreationError",
    [TOKENIZE_func_SEP_OP_TOKEN_CREATION_ERROR] = "SepOpTokenCreationError",
    [TOKENIZE_func_STRING_TOKEN_CREATION_ERROR] = "StringTokenCreationError",
    [GET_NUMBER_TOKEN_func_ALLOC_ERROR] = "NumberTokenAllocError",
    [GET_NUMBER_TOKEN_func_BIG_NUMBER_ERROR] = "BigNumberError",
    [GET_NUMBER_TOKEN_func_INVALID_NUMBER] = "InvalidNumberError",
    [GET_TEXT_TOKEN_func_ALLOC_ERROR] = "IdentifierTokenAllocError",
    [GET_TEXT_TOKEN_func_INVALID_IDENTIFIER_ERROR] = "InvalidIdentifierError",
    [GET_TEXT_TOKEN_func_LONG_IDENTIFIER_ERROR] = "TooLongIdentifierError",
    [GET_TEXT_TOKEN_func_LONG_STRING_ERROR] = "TooLongStringError",
    [GET_TEXT_TOKEN_func_QUOTE_NOT_FOUND_ERROR] = "ClosingQouteNotFoundError",
    [GET_SEP_OP_TOKEN_func_ALLOC_ERROR] = "SepOpTokenAllocError",
    [GET_SEP_OP_TOKEN_func_INVALID_SEP_OP_ERROR] = "InvalidSepOpError",
};

/* returns the error code description */
const char* decode_err(int error_code, bool warning)
{
    const char* const* messages = warning ? warning_messages : error_messages;
    const int count = warning ? WARNINGS_CODES_COUNT : ERRORS_CODES_COUNT;
    
    if(error_code >= 0 && error_code < count) 
        return messages[error_code];
    
    return warning ? "UnknownWarning" : "UnknownError";
}

void add_err_code(int err_code, int line_num, bool warning)
{   
    if(errors_count > MAX_FAILS - 1)
        return;

    errors_array[errors_count].err_code = err_code;
    errors_array[errors_count].line_num = line_num;
    errors_array[errors_count].warning = warning;

    ++errors_count;
}

/* it will be called by atexit() */
void print_errors()
{
    int i;
    for(i = 0; i < errors_count; ++i)
    {
        if(errors_array[i].warning)
        {
            printf(">>> Warning <code:%2hd> in line %hd: %s\n",
                   errors_array[i].err_code, 
                   errors_array[i].line_num, 
                   decode_err(errors_array[i].err_code, true));
        }
        else
        {
            printf(">>> Error   <code:%2hd> in line %hd: %s\n",
                   errors_array[i].err_code, 
                   errors_array[i].line_num, 
                   decode_err(errors_array[i].err_code, false));
        }
    }
}