#include "errs.h"

#include <stdbool.h>
#include <stdio.h>

static int errors_count = 0;
err_code errors_array[MAX_FAILS];

/* returns the error code description */
char* decode_err(int error_code, bool warning)
{
    if(warning)
    {
        #ifdef DECODE_w
        #undef DECODE_w
        #endif

        switch(error_code)
        {
            #define DECODE_w(code, description) case code : return description ;
            WARNINGS
            #undef  DECODE_w

            default                                       : return "UnknownError" ;
        }  
    }    

    else
    {
        #ifdef  DECODE
        #undef  DECODE
        #endif

        switch(error_code)
        {
            #define DECODE(code, description) case code : return description ;
            ERRORS
            #undef  DECODE

            default                                     : return "UnknownError" ;
        } 
    }
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