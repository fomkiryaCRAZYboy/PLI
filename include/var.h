#ifndef VAR_H
#define VAR_H

#include "stdint.h"
#include "parser.h"

#define MAX_STR_SIZE    MAX_TOKEN_TEXT_SIZE
#define MAX_VAR_SIZE    MAX_TOKEN_TEXT_SIZE

/* ---any extra information about literals and variables--- */


/* data types for literals and variables */
typedef enum  { 
                INT_TYPE, 
                FLOAT_TYPE, 
                BOOL_TYPE, 
                STR_TYPE, 
                NONE_TYPE     /* NONE_TYPE for newly declared variables: "var x;" */
              } 
DATA_TYPE ;
            
/* value pattern */
typedef union
{
        int64_t int_value;
        double  float_value;
        char    str_value[MAX_STR_SIZE];
        int     bool_value;    
}
value_t ;

/* variable pattern */
typedef struct
{
    char      var_name[MAX_VAR_SIZE];
    DATA_TYPE var_data_type;
    value_t   var_value;
}
variable_t ;

#endif /* VAR_H */