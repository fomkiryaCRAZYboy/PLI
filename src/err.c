#include "errs.h"

#include <stdio.h>
#include <string.h>

static int errors_count = 0;
ERR_CODE errors_array[MAX_FAILS];

int get_all_funcs()
{

}

char* decode_err(int error_code, char* function_name)
{
    
}

void add_err_code(int err_code, int line_num, char* function_name)
{   
    if(errors_count > MAX_FAILS - 1)
        return;

    if(!function_name)
        function_name = "unknown function";

    errors_array[errors_count].err_code = err_code;
    errors_array[errors_count].line_num = line_num;
    strncpy(errors_array[errors_count].function_name, function_name, sizeof(errors_array[errors_count].function_name));

    ++errors_count;
}

void print_errors()
{
    int i;
    for(i = 0; i < errors_count; ++i)
    {
        printf("ERROR:");
    }
}