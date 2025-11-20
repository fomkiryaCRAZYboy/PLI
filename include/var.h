#ifndef VAR_H
#define VAR_H

/* ---any extra information about variables--- */

#include "parser.h"
#include "stdint.h"

/* data types for literals and variables */
typedef enum data_type { 
                        INT_TYPE, 
                        FLOAT_TYPE, 
                        BOOL_TYPE, 
                        STR_TYPE, 
                        NONE_TYPE     /* NONE_TYPE for newly declared variables: "var x;" */
                       } 
             DATA_TYPE ;
            
typedef union
{
        int64_t int_value;
        double  float_value;
        char    str_value[MAX_STR_SIZE];
        int     bool_value;    
}
VALUE;

/* variable pattern */
typedef struct
{
    char      var_name[MAX_VAR_SIZE];
    DATA_TYPE var_data_type;
    VALUE     var_value;
}
VARIABLE ;


#endif /* VAR_H */