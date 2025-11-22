#ifndef VAR_H
#define VAR_H

#include "stdint.h"
#include "parser.h"

#define MAX_STR_SIZE    MAX_TOKEN_TEXT_SIZE
#define MAX_VAR_SIZE    MAX_TOKEN_TEXT_SIZE

/* ---any extra information about literals and variables--- */


/* data types for literals and variables */
typedef enum { 
                INT_TYPE, 
                FLOAT_TYPE, 
                BOOL_TYPE, 
                STR_TYPE, 
                NONE_TYPE     /* NONE_TYPE for newly declared variables: "var x;" */
             } 
DATA_TYPE ;
          
/* value pattern */
typedef union value
{
        int64_t int_value;
        double  float_value;
        char    str_value[MAX_STR_SIZE];
        short   bool_value;    
}
value_t ;

/* variable pattern */
typedef struct variable
{
    char      var_name[MAX_VAR_SIZE];
    DATA_TYPE var_data_type;
    value_t   var_value;
}
variable_t ;

/* expression pattern */
/* eventually the expression will turn into a value_t */
typedef struct expression
{
    TOKEN* expr;    /* 
                        pointer to a token (stream of tokens) 
                        defining the expression
                    */
}
expression_t;

#endif /* VAR_H */