#ifndef VAR_H
#define VAR_H

#include "lexer.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_STR_SIZE    MAX_TOKEN_TEXT_SIZE
#define MAX_VAR_SIZE    MAX_TOKEN_TEXT_SIZE

/* ============================================================================
 * RUNTIME VALUE TYPES (for interpreter/evaluator)
 * ============================================================================ */

/* Data types for runtime values */
typedef enum 
{ 
    VAL_INT,       /* Integer value */
    VAL_FLOAT,     /* Floating-point value */
    VAL_BOOL,      /* Boolean value */
    VAL_STRING,    /* String value */
    VAL_NONE       /* Uninitialized/undefined value */
} value_type_t;

/* Runtime value union */
typedef union
{
    int64_t int_val;
    double  float_val;
    bool    bool_val;
    char    string_val[MAX_STR_SIZE];
} runtime_value_t;

/* Runtime value with type tag */
typedef struct {
    value_type_t    type;
    runtime_value_t value;
} value_t;

/* Variable entry (for variable storage) */
typedef struct {
    char    name[MAX_VAR_SIZE];
    value_t value;
    bool    is_initialized;
} variable_t;

#endif /* VAR_H */