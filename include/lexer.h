#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKENS_COUNT_IN_BLOCK    1024
#define MAX_TOKEN_TEXT_SIZE          64  /* Если максимальная длна текста токена 64,
                                            то необходимо сделать макс. длину имени переменной 
                                            и макс. длину для string 64 */

/*  */
typedef enum f_create_token_result
{
    SUCCESS_TOKEN_CREATION, 
    STREAM_OVERFLOW_ERROR
} 
F_CREATE_TOKEN_RESULT ;

/*  */
typedef enum f_tokenize_result
{   
    SUCCESS_TOKENIZE,
    STREAM_CREATION_ERROR,
    TOKEN_CREATION_ERROR,
    BIG_NUMBER_ERROR,
}       
F_TOKENIZE_RESULT ;

/*  */
typedef enum 
{
    TOKEN_NUMBER,   /* '25' */ 
    TOKEN_STRING,   /* '"string"' */
    TOKEN_VARIABLE, /* 'variable name' */
    TOKEN_SPACE,    /* ' ' */
    TOKEN_COMMA     /* ',' */
} 
TOKEN_TYPE ;

/*  */
typedef struct token
{
    char        text_token[MAX_TOKEN_TEXT_SIZE] ;
    TOKEN_TYPE  type_token ;
} 
TOKEN ;

/* (block of code) */
F_TOKENIZE_RESULT tokenize(char* block) ;
F_CREATE_TOKEN_RESULT create_token(TOKEN_TYPE token_type, char* token_text, TOKEN* tokens_stream, int* tokens_count_in_stream) ;

#endif /* LEXER_H */