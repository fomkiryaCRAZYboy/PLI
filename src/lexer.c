#include "lexer.h"
#include "mem.h"
#include "errs.h"

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char* get_number_token(char** line_ptr, int current_line)
{
    char* current = *line_ptr;

    char* number_token = pli_alloc(MAX_NUM_SIZE + 1);
    if(!number_token)
    {
        add_err_code(GET_NUMBER_TOKEN_func_ALLOC_ERROR, current_line);
        return NULL ;
    }

    int iter = 0;

    if (*current == '+' || *current == '-')
    {
        number_token[iter++] = *current++;
    }

    while(isdigit(*current) && iter < MAX_NUM_SIZE)
    {
        number_token[iter++] = *current++;
    }

    if (*current == '.' && iter < MAX_NUM_SIZE - 1) {
        number_token[iter++] = *current++;
        
        while(isdigit(*current) && iter < MAX_NUM_SIZE)
        {
            number_token[iter++] = *current++;
        }
    }

    if(iter == MAX_NUM_SIZE && (isdigit(*current) || *current == '.'))
    {
        add_err_code(GET_NUMBER_TOKEN_func_BIG_NUMBER_ERROR, current_line);
        pli_free(number_token);

        return NULL;
    }

    number_token[iter] = '\0';
    
    if (iter == 0 || (iter == 1 && (number_token[0] == '+' || number_token[0] == '-' || number_token[0] == '.'))) 
    {
        add_err_code(GET_NUMBER_TOKEN_func_INVALID_NUMBER, current_line);
        pli_free(number_token);

        return NULL;
    }

    *line_ptr = current;
    return number_token;
}

char* get_iden_token(char** line_ptr, int current_line)
{   
    char* current = *line_ptr;

    char* identifier_token = pli_alloc(MAX_TOKEN_TEXT_SIZE + 1);
    if(!identifier_token)
    {
        add_err_code(GET_IDEN_TOKEN_func_ALLOC_ERROR, current_line);
        return NULL ;
    }

    /* '_'/'__' - invalid identifier name */
    if(*current == '_' && !isalpha(*(current + 1)))
    {
        //add_err_code()
    }

    while(isalpha(*current) || )

    *line_ptr = current;
    return identifier_token;
}

/* create token directly in the tokens stream */
f_result create_token(TOKEN_TYPE token_type, char* token_text, TOKEN_STREAM* stream, int line_number)
{
    if(stream->count >= MAX_TOKENS_COUNT_IN_BLOCK - 1) 
    {
        add_err_code(CREATE_TOKEN_func_STREAM_OVERFLOW_ERROR, line_number);
        return CREATE_TOKEN_func_STREAM_OVERFLOW_ERROR ;
    }    
    stream->tokens[stream->count].type_token = token_type ;
    stream->tokens[stream->count].line_number = line_number ;

    /* strings, var names & kw`s*/
    if(token_type == unknown_token || token_type <= str_token)
        strncpy(stream->tokens[stream->count].text_token.text, token_text, MAX_TOKEN_TEXT_SIZE) ;
    
    /* sep_and_op_text */
    else if(token_type >= math_op_token_mult)
        strncpy(stream->tokens[stream->count].text_token.sep_and_op_text, token_text, MAX_SEP_AND_OP_SIZE) ;

    /* bool_text */
    else if(token_type == bool_token_t || token_type == bool_token_f)
        strncpy(stream->tokens[stream->count].text_token.bool_text, token_text, MAX_BOOL_SIZE) ;

    /* num_text */
    else if(token_type == num_token)
        strncpy(stream->tokens[stream->count].text_token.num_text, token_text, MAX_NUM_SIZE) ;

    ++(stream->count);

    return SUCCESS ;
}


/*  
main tokenize function that 
defines tokens in block of code.
*/
TOKEN_STREAM* tokenize(char* block)
{
    TOKEN_STREAM* stream;
    char* line_ptr;
    int current_line = 1;

    stream = pli_alloc(sizeof(TOKEN_STREAM));
    if(!stream) 
    {
        add_err_code(TOKENIZE_func_STREAM_CREATION_ERROR, current_line);
        return NULL ;
    }

    stream->tokens = pli_alloc(MAX_TOKENS_COUNT_IN_BLOCK * sizeof(TOKEN));
    if(!stream->tokens)
    {
        add_err_code(TOKENIZE_func_STREAM_CREATION_ERROR, current_line);
        pli_free(stream);
        return NULL;
    }

    stream->count = 0;
    stream->current_line = 1;

    line_ptr = block;

    /* main tokenize cycle */
    while(line_ptr)
    {
        /* Skip whitespace and track line numbers */
        while(isspace(*line_ptr))
        {
            if(*line_ptr == '\n')
                current_line++;
            line_ptr++;
        }

        /* end of code block */
        if(*line_ptr == '\0') 
            break; 

        /* starts from '+'/'-'/digit --> number*/  
        /* numbers handling */  
        if(isdigit (*line_ptr) || *line_ptr == '+' || *line_ptr == '-') 
        {
            char* text_token = get_number_token(&(line_ptr), current_line);
            if(!text_token)
            {
                add_err_code(TOKENIZE_func_GET_NUMBER_TOKEN_ERROR, current_line);
                pli_free(stream->tokens);
                pli_free(stream);

                return NULL ;
            }

            if(create_token(num_token, text_token, stream, current_line) != SUCCESS)
            {
                add_err_code(TOKENIZE_func_NUM_TOKEN_CREATION_ERROR, current_line);

                pli_free(stream->tokens);
                pli_free(stream);
                pli_free(text_token);

                return NULL ;
            }

            pli_free(text_token);
        }

        /* true/_name/print */
        /* bool values/identifers/keywords handling */
        if(isalpha (*line_ptr) || *line_ptr == '_')
        {
            /* getting variable name */
            if(*line_ptr == '_')
            {

            }
        }

        ++line_ptr;

    }

    stream->current_line = current_line;
    return stream ;
}