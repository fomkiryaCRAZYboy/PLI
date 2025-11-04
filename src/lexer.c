#include "lexer.h"
#include "mem.h"
#include "errs.h"

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char* get_number_token(char** line_ptr)
{
    char* current = *line_ptr;

    char* num_token = pli_alloc(MAX_NUM_SIZE + 1);
    if(!num_token)
    {
        add_err_code(GET_NUMBER_TOKEN_func_ALLOC_ERROR, 000000);
        return NULL ;
    }

    int iter = 0;

    if (*current == '+' || *current == '-')
    {
        num_token[iter++] = *current++;
    }

    while(isdigit(*current) && iter < MAX_NUM_SIZE)
    {
        num_token[iter++] = *current++;
    }

    if (*current == '.' && iter < MAX_NUM_SIZE - 1) {
        num_token[iter++] = *current++;
        
        while(isdigit(*current) && iter < MAX_NUM_SIZE)
        {
            num_token[iter++] = *current++;
        }
    }

    if(iter == MAX_NUM_SIZE && (isdigit(*current) || *current == '.'))
    {
        add_err_code(GET_NUMBER_TOKEN_func_BIG_NUMBER_ERROR, 00000);
        pli_free(num_token);

        return NULL;
    }

    num_token[iter] = '\0';
    
    if (iter == 0 || (iter == 1 && (num_token[0] == '+' || num_token[0] == '-' || num_token[0] == '.'))) 
    {
        add_err_code(GET_NUMBER_TOKEN_func_INVALID_NUMBER, 00000);
        pli_free(num_token);

        return NULL;
    }

    *line_ptr = current;
    return num_token;
}

/* create token directly in the tokens stream */
f_result create_token(TOKEN_TYPE token_type, char* token_text, TOKEN* tokens_stream, int* tokens_count_in_stream)
{
    if(*tokens_count_in_stream >= MAX_TOKENS_COUNT_IN_BLOCK - 1) 
    {
        add_err_code(CREATE_TOKEN_func_STREAM_OVERFLOW_ERROR, 0000000);
        return CREATE_TOKEN_func_STREAM_OVERFLOW_ERROR ;
    }    
    tokens_stream[*tokens_count_in_stream].type_token = token_type ;

    if(token_type == unknown_token)
    {
        strncpy(tokens_stream[*tokens_count_in_stream].text_token.text, token_text, MAX_TOKEN_TEXT_SIZE) ;
    }
    
    /* sep_and_op_text */
    else if(token_type >= math_op_token_mult)
    {
        strncpy(tokens_stream[*tokens_count_in_stream].text_token.sep_and_op_text, token_text, MAX_SEP_AND_OP_SIZE) ;
    }

    /* bool_text */
    else if(token_type == bool_token_t || token_type == bool_token_f)
    {
        strncpy(tokens_stream[*tokens_count_in_stream].text_token.bool_text, token_text, MAX_BOOL_SIZE) ;
    }

    /* num_text */
    else if(token_type == num_token)
    {
        strncpy(tokens_stream[*tokens_count_in_stream].text_token.num_text, token_text, MAX_NUM_SIZE) ;
    }

    /* strings, var names & kw`s*/
    else if(token_type < 12)
    {
        strncpy(tokens_stream[*tokens_count_in_stream].text_token.text, token_text, MAX_TOKEN_TEXT_SIZE) ;
    }

    ++(*tokens_count_in_stream);

    return SUCCESS ;
}


/*  
main tokenize function that 
defines tokens in block of code.
*/
TOKEN* tokenize(char* block, int* tokens_count)
{
    TOKEN* tokens_stream;
    int iterator = 0;
    char* line_ptr;

    tokens_stream = pli_alloc (MAX_TOKENS_COUNT_IN_BLOCK * sizeof(TOKEN)) ; 
    if(!tokens_stream) 
    {
        add_err_code(TOKENIZE_func_STREAM_CREATION_ERROR, 00000000);
        return NULL ;
    }

    iterator = 0;
    line_ptr = block;

    *tokens_count = 0;

    
    /* main tokenize cycle */
    while(line_ptr)
    {
        while(isspace (*line_ptr)) 
            line_ptr++ ;

        /* end of code block */
        if(*line_ptr == '\0') break; 

        /* starts from '+'/'-'/digit --> number*/    
        if(isdigit (*line_ptr) || *line_ptr == '+' || *line_ptr == '-') 
        {
            char* text_token = get_number_token(&(line_ptr));
            if(!text_token)
            {
                add_err_code(TOKENIZE_func_GET_NUMBER_TOKEN_ERROR, 000000);
                pli_free(tokens_stream);

                return NULL ;
            }

            if(create_token(num_token, text_token, tokens_stream, tokens_count) != SUCCESS)
            {
                add_err_code(TOKENIZE_func_NUM_TOKEN_CREATION_ERROR, 000000);

                pli_free(tokens_stream);
                pli_free(text_token);

                return NULL ;
            }

            pli_free(text_token);
        }

    }

    return tokens_stream ;
}