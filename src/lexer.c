#include "lexer.h"
#include "mem.h"
#include "errs.h"

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

bool is_float_num(char* line_ptr)
{
    int total_digits = 0;
    int meaningful_digits = 0;
    bool found_point = false;
    bool found_nonzero = false;
    int i = 0;
    
    const int MAX_MEANINGFUL_DIGITS = 15;
    
    if (line_ptr[i] == '+' || line_ptr[i] == '-') {
        i++;
    }
    
    while (isdigit(line_ptr[i]) && total_digits < MAX_FLOAT_SIZE) {
        if (line_ptr[i] != '0') {
            found_nonzero = true;
        }
        if (found_nonzero && meaningful_digits < MAX_MEANINGFUL_DIGITS) {
            meaningful_digits++;
        }
        total_digits++;
        i++;
    }
    
    if (line_ptr[i] == '.') {
        found_point = true;
        i++;
        
        while (isdigit(line_ptr[i]) && total_digits < MAX_FLOAT_SIZE) {
            if (meaningful_digits < MAX_MEANINGFUL_DIGITS) {
                meaningful_digits++;
                found_nonzero = true; 
            }
            total_digits++;
            i++;
        }
    }
    
    return (found_point && found_nonzero && total_digits <= MAX_FLOAT_SIZE);
}

char* get_number_token(char* line_ptr)
{
}

/* create token directly in the tokens stream */
f_result create_token(TOKEN_TYPE token_type, char* token_text, TOKEN* tokens_stream, int* tokens_count_in_stream)
{
    if(*tokens_count_in_stream >= MAX_TOKENS_COUNT_IN_BLOCK - 1) 
    {
        add_err_code(CREATE_TOKEN_func_STREAM_OVERFLOW_ERR, 0000000);
        return CREATE_TOKEN_func_STREAM_OVERFLOW_ERR ;
    }    
    tokens_stream[*tokens_count_in_stream].type_token = token_type ;
    strncpy(tokens_stream[*tokens_count_in_stream].text_token, token_text, MAX_TOKEN_TEXT_SIZE) ;

    ++(*tokens_count_in_stream);

    return SUCCESS ;
}


/*  
main tokenize function that 
defines tokens in block of code.
*/
f_result tokenize(char* block)
{
    TOKEN* tokens_stream = NULL;
    int iterator = 0;
    int tokens_count_in_stream = 0;
    char token_text[MAX_TOKEN_TEXT_SIZE];
    char* line_ptr = NULL;

    tokens_stream = pli_alloc (MAX_TOKENS_COUNT_IN_BLOCK * sizeof(TOKEN)) ; 
    if(!tokens_stream) 
    {
        add_err_code(TOKENIZE_func_STREAM_CREATION_ERROR, 0000000000);
        return TOKENIZE_func_STREAM_CREATION_ERROR ;
    }

    iterator = 0;
    tokens_count_in_stream = 0;
    line_ptr = block;
    
    /* main tokenize cycle */
    while(line_ptr)
    {
        while(isspace (*line_ptr)) 
            line_ptr++ ;

        if(isdigit (*line_ptr)) 
        {
            get_number_token(line_ptr);
            while(isdigit (*line_ptr) && iterator < MAX_NUMBER_DIGITS) 
            {
                token_text[iterator++] = *line_ptr++ ;
            }
            
            if(iterator < MAX_NUMBER_DIGITS)
                token_text[iterator] = '\0' ;
            else
            {
                add_err_code(TOKENIZE_func_BIG_NUMBER_ERROR, 0000000);
                return TOKENIZE_func_BIG_NUMBER_ERROR ;
            }    
            if(create_token (num_token, token_text, tokens_stream, &tokens_count_in_stream) != SUCCESS)
            {
                add_err_code(TOKENIZE_func_TOKEN_CREATION_ERROR, 000000);
                return TOKENIZE_func_TOKEN_CREATION_ERROR ;
            }
        }

    }

    
    return SUCCESS ;
}