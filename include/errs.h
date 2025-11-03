#ifndef ERR_H
#define ERR_H

#include <stddef.h>

#define MAX_FAILS   20


#define ERRORS \
        DECODE(SUCCESS,                               "Success") \
        \
        DECODE(CREATE_TOKEN_func_STREAM_OVERFLOW_ERR, "TokenStreamOverflowError") \
        \
        DECODE(TOKENIZE_func_STREAM_CREATION_ERROR,   "StreamCreationError") \
        DECODE(TOKENIZE_func_TOKEN_CREATION_ERROR,    "TokenCreationError") \
        DECODE(TOKENIZE_func_BIG_NUMBER_ERROR,        "BigNumberError") \


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