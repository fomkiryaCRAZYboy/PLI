#ifndef VAR_H
#define VAR_H

#include "lexer.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_STR_SIZE        MAX_TOKEN_TEXT_SIZE
#define MAX_VAR_SIZE        MAX_TOKEN_TEXT_SIZE
#define HASHMAP_INIT_CAP    8
#define HASHMAP_LOAD_FACTOR 0.75


/* ============================================================================
 * RUNTIME VALUE TYPES
 * ============================================================================ */

typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_STRING,
    VAL_NONE
} value_type_t;

typedef union {
    int64_t int_val;
    double  float_val;
    bool    bool_val;
    char    string_val[MAX_STR_SIZE];
} runtime_value_t;

typedef struct {
    value_type_t    type;
    runtime_value_t value;
} value_t;


/* ============================================================================
 * VARIABLE ENTRY
 * name[0] == '\0'  → empty slot
 * name[0] == '\x7f' → tombstone (deleted)
 * ============================================================================ */

typedef struct {
    char    name[MAX_VAR_SIZE];
    value_t value;
    bool    is_const;
} variable_t;


/* ============================================================================
 * HASH MAP
 * ============================================================================ */

typedef struct {
    variable_t *entries;
    size_t      capacity;
    size_t      count;       /* active entries (excludes tombstones) */
} hashmap;

uint32_t    var_hash_function(const char *varname);

hashmap    *hashmap_new(void);
void        hashmap_free(hashmap *map);
bool        hashmap_set(hashmap *map, const char *name, value_t val, bool is_const);
variable_t *hashmap_lookup(hashmap *map, const char *name);
bool        hashmap_delete(hashmap *map, const char *name);

#endif /* VAR_H */