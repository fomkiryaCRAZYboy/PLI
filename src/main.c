#include "lexer.h"
#include "mem.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {    
    atexit (print_errors);      /* 
                                    If there are no errors, 
                                    the output will be empty.     
                                */
    atexit (emergeny_cleanup);  /* 
                                    If the memory is not occupied, 
                                    nothing will happen 
                                */

    char program[] = "77ю";
    
    int actual_tokens_count = 0;
    int* tokens_count = &actual_tokens_count;
    TOKEN* stream;

    stream = tokenize(program, tokens_count);
    if(!stream)
    {
        add_err_code(MAIN_func_TOKENIZE_ERROR, 00000);
        exit(EXIT_FAILURE);
    }

    debug_print_stream(stream, tokens_count);
    
    exit(EXIT_SUCCESS);
}