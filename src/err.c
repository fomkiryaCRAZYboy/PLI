#include "errs.h"

#include <stdio.h>
#include <string.h>

static int errors_count = 0;
err_code errors_array[MAX_FAILS];

/* returns the error code description */
char* decode_err(int error_code)
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


void add_err_code(int err_code, int line_num)
{   
    if(errors_count > MAX_FAILS - 1)
        return;

    errors_array[errors_count].err_code = err_code;
    errors_array[errors_count].line_num = line_num;

    ++errors_count;
}

/* it will be called by atexit() */
void print_errors()
{
    int i;
    for(i = 0; i < errors_count; ++i)
    {
        printf("Error <%hd> in line <%hd>: %s\n",
               errors_array[i].err_code, 
               errors_array[i].line_num, 
               decode_err(errors_array[i].err_code));
    }
}