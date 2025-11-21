#ifndef OPS_H
#define OPS_H

typedef enum { 
            ADD,   /* addition '+' */
            SUB,   /* subtraction '-' */
            MULT,  /* multiplication '*' */
            DIV    /* division '/' */
            }  
MATH_OP;

typedef enum { 
            EQ,        /* equal '=' */
            NEQ,       /* not equal '!=' */
            GREAT,     /* greater than '>' */
            LESS,      /* less than '<' */
            GREAT_EQ,  /* greater than or equal '>=' */
            LESS_EQ    /* less than or equal '<=' */
            }  
BIN_OP;

typedef enum { 
            AND,   /* logical and */
            OR,    /* logical or */
            NOT    /* logical not */
            }  
LOGICAL_OP;

typedef enum { 
            READ,     /* print operation */
            PRINT     /* read operation */
            }  
IO_OP;

#endif /* OPS_H */