#include "lexer.h"
#include "mem.h"
#include "errs.h"

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Convert text to token type (keywords, booleans, or identifier) */
static TOKEN_TYPE text_to_token_type(char* token_text)
{
    if(!token_text)
        return unknown_token;

    switch(token_text[0])
    {
        case 'a':
            if(strcmp(token_text, "and") == 0) return kw_token_and;
            break;
        case 'e':
            if(strcmp(token_text, "else") == 0) return kw_token_else;
            if(strcmp(token_text, "end") == 0) return kw_token_end;
            break;
        case 'f':
            if(strcmp(token_text, "false") == 0) return bool_token_f;
            break;
        case 'i':
            if(strcmp(token_text, "if") == 0) return kw_token_if;
            break;
        case 'n':
            if(strcmp(token_text, "not") == 0) return kw_token_not;
            break;
        case 'o':
            if(strcmp(token_text, "or") == 0) return kw_token_or;
            break;
        case 'p':
            if(strcmp(token_text, "print") == 0) return kw_token_print;
            break;
        case 'r':
            if(strcmp(token_text, "read") == 0) return kw_token_read;
            break;
        case 't':
            if(strcmp(token_text, "true") == 0) return bool_token_t;
            break;
        case 'v':
            if(strcmp(token_text, "var") == 0) return kw_token_var;
            break;
        case 'w':
            if(strcmp(token_text, "while") == 0) return kw_token_while;
            break;
    }

    /* Default: identifier */
    return iden_token;
}

/* Convert operator/separator string to token type */
static TOKEN_TYPE operator_to_token_type(char* op_sep_text)
{
    if(!op_sep_text)
        return unknown_token;

    /* Check two-character operators first */
    if(op_sep_text[1] != '\0')
    {
        if(strcmp(op_sep_text, "==") == 0) return comp_op_token_equal;
        if(strcmp(op_sep_text, "!=") == 0) return comp_op_token_not_equal;
        if(strcmp(op_sep_text, ">=") == 0) return comp_op_token_more_equal;
        if(strcmp(op_sep_text, "<=") == 0) return comp_op_token_less_equal;
        
        return unknown_token;
    }

    /* Single-character operators and separators */
    switch(op_sep_text[0])
    {
        /* Arithmetic operators */
        case '*': return math_op_token_mult;
        case '/': return math_op_token_div;
        case '+': return math_op_token_plus;
        case '-': return math_op_token_minus;

        /* Assignment operator */
        case '=': return assing_op_token;

        /* Comparison operators */
        case '>': return comp_op_token_more;
        case '<': return comp_op_token_less;

        /* Separators */
        case '(': return sep_token_lparen;
        case ')': return sep_token_rparen;
        case '{': return sep_token_lbrace;
        case '}': return sep_token_rbrace;
        case ',': return sep_token_comma;
        case ';': return sep_token_semicolon;
        case '.': return sep_token_dot;

        default:
            return unknown_token;
    }
}

char* get_op_sep_token(char** line_ptr, int current_line)
{
    char* current = *line_ptr;

    char* sep_op_token = pli_alloc(MAX_SEP_AND_OP_SIZE + 1);
    if(!sep_op_token)
    {
        add_err_code(GET_SEP_OP_TOKEN_func_ALLOC_ERROR, current_line, false);
        return NULL ;
    }

    switch (current[0])
    {
    case '*':
        sep_op_token[0] = '*';
        goto one_sym;
    case '/':
        sep_op_token[0] = '/';
        goto one_sym;
    case '+':
        sep_op_token[0] = '+';
        goto one_sym;
    case '-':
        sep_op_token[0] = '-';
        goto one_sym;
    case '=':
        if(current[1] == '=') goto two_sym;
        
        sep_op_token[0] = '=';
        goto one_sym;
    case '>':
        if(current[1] == '=') goto two_sym;

        sep_op_token[0] = '>';
        goto one_sym;
    case '<':
        if(current[1] == '=') goto two_sym;

        sep_op_token[0] = '<';
        goto one_sym;
    case '(':
        sep_op_token[0] = '(';
        goto one_sym;
    case ')':
        sep_op_token[0] = ')';
        goto one_sym;
    case '{':
        sep_op_token[0] = '{';
        goto one_sym;
    case '}':
        sep_op_token[0] = '}';
        goto one_sym;
    case ',':
        sep_op_token[0] = ',';
        goto one_sym;
    case ';':
        sep_op_token[0] = ';';
        goto one_sym;
    case '.':
        sep_op_token[0] = '.';
        goto one_sym;
    case '!':  
        if(current[1] == '=') goto two_sym;
        /* single '!' is not valid operator */
        break;

    default:
        break;
    }

    /* error - unknown operator/separator */
    pli_free(sep_op_token);
    return NULL;

two_sym:
    sep_op_token[0] = current[0];
    sep_op_token[1] = current[1];
    sep_op_token[2] = '\0';
    *line_ptr = current + 2;  /* move pointer forward by 2 */
    return sep_op_token;

one_sym:
    sep_op_token[1] = '\0';
    *line_ptr = current + 1;  /* move pointer forward by 1 */
    return sep_op_token;
}

char* get_number_token(char** line_ptr, int current_line)
{
    char* current = *line_ptr;

    char* number_token = pli_alloc(MAX_NUM_SIZE + 1);
    if(!number_token)
    {
        add_err_code(GET_NUMBER_TOKEN_func_ALLOC_ERROR, current_line, false);
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
        add_err_code(GET_NUMBER_TOKEN_func_BIG_NUMBER_ERROR, current_line, false);
        pli_free(number_token);

        return NULL;
    }

    number_token[iter] = '\0';
    
    if (iter == 0 || (iter == 1 && (number_token[0] == '+' || number_token[0] == '-' || number_token[0] == '.'))) 
    {
        add_err_code(GET_NUMBER_TOKEN_func_INVALID_NUMBER, current_line, false);
        pli_free(number_token);

        return NULL;
    }

    *line_ptr = current;
    return number_token;
}

static bool next_is_undrline(char* p) { return *(p + 1) == '_'; } 

/* get idens, strings, kwords */
char* get_text_token(char** line_ptr, int current_line)
{   
    char* current = *line_ptr;

    char* text_token = pli_alloc(MAX_TOKEN_TEXT_SIZE + 1);
    if(!text_token)
    {
        add_err_code(GET_TEXT_TOKEN_func_ALLOC_ERROR, current_line, false);
        return NULL ;
    }

    int iter = 0;

    /* string literal handling: starts with " */
    if(*current == '"')
    {
        text_token[iter++] = *current++;  /* add opening " */
        
        /* read all characters until closing " */
        while(*current != '\0' && *current != '"' && iter < MAX_TOKEN_TEXT_SIZE)
        {
            text_token[iter++] = *current++;
        }
        
        /* check if string is too long */
        if(iter == MAX_TOKEN_TEXT_SIZE && *current != '"' && *current != '\0')
        {
            add_err_code(GET_TEXT_TOKEN_func_LONG_IDENTIFIER_ERROR, current_line, false);
            pli_free(text_token);
            return NULL;
        }
        
        /* add closing " if found */
        if(*current == '"' && iter < MAX_TOKEN_TEXT_SIZE)
        {
            text_token[iter++] = *current++;
        }
        /* if string is unterminated (*current == '\0'), we just return what we have */
        
        text_token[iter] = '\0';
        *line_ptr = current;
        return text_token;
    }

    /* curr symbol is '_' or alpha */
    while((isalnum(*current) || *current == '_') && iter < MAX_TOKEN_TEXT_SIZE)
    {
        /* double underline "__"  -> invalid iden*/
        if(*current == '_' && *(current + 1) != '\0' && next_is_undrline(current))
        {
            add_err_code(GET_TEXT_TOKEN_func_INVAILD_IDENTIFIER_ERROR, current_line, false);
            pli_free(text_token);
            return NULL;
        }

        text_token[iter++] = *current++;
    }

    /* too long iden name -> error */
    if(iter == MAX_TOKEN_TEXT_SIZE && (isalpha(*current) || *current == '_'))
    {
        add_err_code(GET_TEXT_TOKEN_func_LONG_IDENTIFIER_ERROR, current_line, false);
        pli_free(text_token);
        return NULL;
    }

    text_token[iter] = '\0';

    /* '_'  -> invalid iden*/
    if(iter == 0 || (iter == 1 && text_token[0] == '_'))
    {
        add_err_code(GET_TEXT_TOKEN_func_INVAILD_IDENTIFIER_ERROR, current_line, false);
        pli_free(text_token);
        return NULL;
    }


    *line_ptr = current;
    return text_token;
}

/* create token directly in the tokens stream */
f_result create_token(TOKEN_TYPE token_type, char* token_text, TOKEN_STREAM* stream, int line_number)
{
    if(stream->count >= MAX_TOKENS_COUNT_IN_BLOCK - 1) 
    {
        add_err_code(CREATE_TOKEN_func_STREAM_OVERFLOW_ERROR, line_number, false);
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
Helper function to process token: get token text, create token, handle errors
Returns: SUCCESS on success, error code on failure
On failure, returns error code (cleanup is done by caller via goto clean)
*/
static f_result process_token(
    char* (*get_token_func)(char**, int),
    TOKEN_TYPE token_type,
    char** line_ptr,
    int current_line,
    TOKEN_STREAM* stream,
    int get_token_error_code,
    int create_token_error_code)
{
    char* token_text = get_token_func(line_ptr, current_line);
    if(!token_text)
    {
        add_err_code(get_token_error_code, current_line, false);
        return get_token_error_code;
    }

    if(create_token(token_type, token_text, stream, current_line) != SUCCESS)
    {
        add_err_code(create_token_error_code, current_line, false);
        pli_free(token_text);
        return create_token_error_code;
    }

    pli_free(token_text);
    return SUCCESS;
}

/* 
Helper function to process text tokens (identifiers, keywords, booleans):
get token text, identify type, create token, handle errors
Returns: SUCCESS on success, error code on failure
On failure, returns error code (cleanup is done by caller via goto clean)
*/
static f_result process_text_token(
    char** line_ptr,
    int current_line,
    TOKEN_STREAM* stream,
    int error_code)
{
    char* token_text = get_text_token(line_ptr, current_line);
    if(!token_text)
    {
        add_err_code(error_code, current_line, false);
        return error_code;
    }

    TOKEN_TYPE token_type = text_to_token_type(token_text);

    if(create_token(token_type, token_text, stream, current_line) != SUCCESS)
    {
        add_err_code(error_code, current_line, false);
        pli_free(token_text);
        return error_code;
    }

    pli_free(token_text);
    return SUCCESS;
}

/* 
Helper function to process operator/separator tokens:
get token text, identify type, create token, handle errors
Returns: SUCCESS on success, error code on failure
On failure, returns error code (cleanup is done by caller via goto clean)
*/
static f_result process_operator_token(
    char** line_ptr,
    int current_line,
    TOKEN_STREAM* stream,
    int error_code)
{
    char* token_text = get_op_sep_token(line_ptr, current_line);
    if(!token_text)
    {
        add_err_code(error_code, current_line, false);
        return error_code;
    }

    TOKEN_TYPE token_type = operator_to_token_type(token_text);

    if(create_token(token_type, token_text, stream, current_line) != SUCCESS)
    {
        add_err_code(error_code, current_line, false);
        pli_free(token_text);
        return error_code;
    }

    pli_free(token_text);
    return SUCCESS;
}

/*  
main tokenize function that 
defines tokens in block of code.
*/
TOKEN_STREAM* tokenize(char* block)
{
    TOKEN_STREAM* stream = NULL;
    char* line_ptr;
    int current_line = 1;

    stream = pli_alloc(sizeof(TOKEN_STREAM));
    if(!stream) 
    {
        add_err_code(TOKENIZE_func_STREAM_CREATION_ERROR, current_line, false);
        goto clean;
    }

    stream->tokens = pli_alloc(MAX_TOKENS_COUNT_IN_BLOCK * sizeof(TOKEN));
    if(!stream->tokens)
    {
        add_err_code(TOKENIZE_func_STREAM_CREATION_ERROR, current_line, false);
        goto clean;
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
            if(process_token(get_number_token, num_token, &line_ptr, current_line, stream,
                           TOKENIZE_func_GET_NUMBER_TOKEN_ERROR,
                           TOKENIZE_func_NUM_TOKEN_CREATION_ERROR) != SUCCESS)
            {
                goto clean;
            }
        }

        /* string literals handling */
        else if(*line_ptr == '"')
        {
            if(process_token(get_text_token, str_token, &line_ptr, current_line, stream,
                           TOKENIZE_func_TEXT_TOKEN_CREATION_ERROR,
                           TOKENIZE_func_TEXT_TOKEN_CREATION_ERROR) != SUCCESS)
            {
                goto clean;
            }
        }

        /* true/_name/print */
        /* bool values/identifers/keywords handling */
        else if(isalpha (*line_ptr) || *line_ptr == '_')
        {
            if(process_text_token(&line_ptr, current_line, stream,
                                 TOKENIZE_func_TEXT_TOKEN_CREATION_ERROR) != SUCCESS)
            {
                goto clean;
            }
        }

        /* separators and operations handling */
        else
        {
            if(process_operator_token(&line_ptr, current_line, stream, 
                                     GET_SEP_OP_TOKEN_func_INVALID_SEP_OP_ERROR) != SUCCESS)
            {
                goto clean;
            }
        }

        ++line_ptr;

    }

    stream->current_line = current_line;
    return stream;

clean:
    if(stream)
    {
        if(stream->tokens)
            pli_free(stream->tokens);
        pli_free(stream);
    }
    return NULL;
}