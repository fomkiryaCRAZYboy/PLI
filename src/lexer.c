#include "lexer.h"
#include "mem.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* create token directly in the tokens stream */
f_result create_token(TOKEN_TYPE token_type, char* token_text, TOKEN* tokens_stream, int* tokens_count_in_stream)
{
    if(*tokens_count_in_stream >= MAX_TOKENS_COUNT_IN_BLOCK - 1) 
        return CREATE_TOKEN_func_STREAM_OVERFLOW_ERR ;

    tokens_stream[*tokens_count_in_stream].type_token = token_type ;
    strncpy(tokens_stream[*tokens_count_in_stream].text_token, token_text, MAX_TOKEN_TEXT_SIZE) ;

    ++(*tokens_count_in_stream);

    return SUCCESS ;
}


/* main tokenize function that defines tokens in block of code */
f_result tokenize(char* line)
{
    TOKEN* tokens_stream = NULL;
    int iterator = 0;
    int tokens_count_in_stream = 0;
    char token_text[MAX_TOKEN_TEXT_SIZE];
    char* line_ptr = NULL;

    tokens_stream = pli_alloc (MAX_TOKENS_COUNT_IN_BLOCK * sizeof(TOKEN)) ; 
    if(!tokens_stream) 
        return TOKENIZE_func_STREAM_CREATION_ERROR ;

    iterator = 0;
    tokens_count_in_stream = 0;
    line_ptr = line;
    
    /* main tokenize cycle */
    while(line_ptr)
    {
        while(isspace (*line_ptr)) 
            line_ptr++ ;

        if(isdigit (*line_ptr)) 
        {
            while(isdigit (*line_ptr) && iterator < MAX_NUMBER_DIGITS) 
            {
                token_text[iterator++] = *line_ptr++ ;
            }
            
            if(iterator < MAX_NUMBER_DIGITS)
                token_text[iterator] = '\0' ;
            else
                return TOKENIZE_func_BIG_NUMBER_ERROR ;

            if(create_token (TOKEN_NUMBER, token_text, tokens_stream, &tokens_count_in_stream) != SUCCESS)
                return TOKENIZE_func_TOKEN_CREATION_ERROR ;
        }

    }

    
    return SUCCESS ;
}