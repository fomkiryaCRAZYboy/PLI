#ifndef ERR_H
#define ERR_H

#include <stddef.h>

#define MAX_FAILS   20

#define WARNINGS \
        DECODE_w(PLI_FREE_func_UNTRACKED_POINTER,       "TryingToFreeUntrackedPointer")


#define ERRORS \
        DECODE(SUCCESS,                                 "Success") \
        \
        DECODE(MAIN_func_TOKENIZE_ERROR,                "TokenizeError") \
        \
        DECODE(CREATE_TOKEN_func_STREAM_OVERFLOW_ERROR, "TokenStreamOverflowError") \
        \
        DECODE(TOKENIZE_func_STREAM_CREATION_ERROR,     "StreamCreationError") \
        DECODE(TOKENIZE_func_GET_NUMBER_TOKEN_ERROR,    "GetNumberTokenError") \
        DECODE(TOKENIZE_func_NUM_TOKEN_CREATION_ERROR,  "NumberTokenCreationError") \
        \
        DECODE(GET_NUMBER_TOKEN_func_ALLOC_ERROR,       "NumberTokenAllocError") \
        DECODE(GET_NUMBER_TOKEN_func_BIG_NUMBER_ERROR,  "BigNumberError") \
        DECODE(GET_NUMBER_TOKEN_func_INVALID_NUMBER,    "InvalidNumberError") \
        \
        DECODE(GET_IDEN_TOKEN_func_ALLOC_ERROR,         "IdentifierTokenAllocError") \
        DECODE(GET_IDEN_TOKEN_func_INVAILD_IDENTIFIER,  "InvalidIdentifierError")

/* generate enum for all kinds of warnings */        
typedef enum
{
    #ifdef  DECODE_w
    #undef  DECODE_w
    #endif

    SUCCESS,
    #define DECODE_w(code, description) code,
    WARNINGS
    #undef  DECODE_w
    CODES_COUNT
} 
warnings ;

/* generate enum for all kinds of errors */
typedef enum
{
    #ifdef  DECODE
    #undef  DECODE
    #endif

    #define DECODE(code, description) code,
    ERRORS
    #undef  DECODE
    CODES_COUNT
} 
f_result ;

/*  the structure stores the error 
    code and the function that 
    terminated with this error */
typedef struct 
{
    short  err_code;
    short  line_num;
}
err_code ;

void add_err_code(int error_code, int line_num);
void print_errors();
char* decode_err(int error_code);

#endif /* ERR_H */