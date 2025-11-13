#include "lexer.h"
#include "mem.h"
#include "main_head.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef FILE_INPUT
#define input_file "test_lexer_seps_ops/input1.pli"
#endif

int main() {    
    atexit (print_errors);      /* 
                                    If there are no errors, 
                                    the output will be empty.     
                                */
    atexit (emergency_cleanup);  /* 
                                    If the memory is not occupied, 
                                    nothing will happen 
                                */
                               
    char program[MAX_TOKENS_COUNT_IN_BLOCK];
    memset(program, 0, sizeof(program));

    TOKEN_STREAM* stream;
                        
#ifdef FILE_INPUT
    FILE* input_f = fopen(input_file, "r");
    if(!input_f)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", input_file);
        return 1;
    }

    /* Read entire file content line by line */
    size_t total_read = 0;
    char buffer[MAX_TOKENS_COUNT_IN_BLOCK];
    
    while(fgets(buffer, sizeof(buffer), input_f) != NULL && total_read < MAX_TOKENS_COUNT_IN_BLOCK - 1)
    {
        size_t line_length = strlen(buffer);
        size_t available = MAX_TOKENS_COUNT_IN_BLOCK - total_read - 1;
        
        if(line_length > available)
            line_length = available;
        
        memcpy(program + total_read, buffer, line_length);
        total_read += line_length;
        
        if(total_read >= MAX_TOKENS_COUNT_IN_BLOCK - 1)
            break;
    }
    
    fclose(input_f);
    program[total_read] = '\0';
#endif /* FILE_INPUT */

    stream = tokenize(program);
    if(!stream)
    {
        add_err_code(MAIN_func_TOKENIZE_ERROR, 0, false);
        exit(EXIT_FAILURE);
    }

    debug_print_stream(stream);

    if(stream)
    {
        if(stream->tokens)
            pli_free(stream->tokens);

        pli_free(stream);
    }
        
    exit(EXIT_SUCCESS);
}