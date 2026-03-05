#include "executor.h"
#include "parser.h"
#include "errs.h"
#include "parser_api.h"
#include "var.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  "true"
#define FALSE "false"
#define NONE  "none"

#define process_error(code, line, warn) \
    do { \
        add_err_code(code, line, warn); \
        return false; \
       } \
    while (0)

extern hashmap* global_map;

bool execute_var_decl(var_decl_stmt_t* var_decl_stmt, int line)
{
    if (!var_decl_stmt)
        process_error(EXECUTE_VAR_DECL_func_VAR_DECL_STMT_IS_NULL, line, false);

    if (!var_decl_stmt->initializer)
        process_error(EXECUTE_VAR_DECL_func_INITIALIZER_IS_NULL, line, false);

    if (!global_map)
        global_map = hashmap_new();
    if (!global_map)
        process_error(HASHMAP_INIT_ERROR, line, false);

    if (hashmap_lookup(global_map, var_decl_stmt->var_name))
        process_error(EXECUTE_VAR_DECL_func_REDECLARATION_ERROR, line, false);

    value_t *val = eval(var_decl_stmt->initializer);
    if (!val)
        process_error(EXECUTE_VAR_DECL_func_INITIALIZER_EVAL_ERROR, line, false);

    if (!hashmap_set(global_map, var_decl_stmt->var_name, *val, false))
        process_error(VAR_SET_ERROR, line, false);

    return true;
}

bool execute_assignment(assignment_stmt_t* assign_stmt, int line)
{
    if (!assign_stmt)
        process_error(EXECUTE_ASSIGNMENT_func_ASSIGN_STMT_IS_NULL, line, false);

    if (!assign_stmt->value)
        process_error(EXECUTE_ASSIGNMENT_func_ASSIGN_VAL_IS_NULL, line, false);

    if (!global_map)
        process_error(EXECUTE_ASSIGNMENT_func_UNDEFINED_VARIABLE, line, false);

    variable_t *var = hashmap_lookup(global_map, assign_stmt->var_name);

    if (!var)
        process_error(EXECUTE_ASSIGNMENT_func_UNDEFINED_VARIABLE, line, false);

    if (var->is_const)
        process_error(EXECUTE_ASSIGNMENT_func_CONST_REASSIGN, line, false);

    value_t *val = eval(assign_stmt->value);
    if (!val)
        process_error(EXECUTE_ASSIGNMENT_func_EVAL_ERROR, line, false);

    var->value = *val;

    return true;
}

static void print_value(value_t* v, bool line_break)
{
    switch (v->type)
    {
        case VAL_INT:
        {
            if(line_break) printf("%" PRId64 "\n", v->value.int_val);
            else           printf("%" PRId64 " ", v->value.int_val);
            break;
        }
        case VAL_FLOAT:
        {
            if(line_break) printf("%g\n", v->value.float_val);
            else           printf("%g ", v->value.float_val);
            break;
        }
        case VAL_BOOL:
        {
            if(line_break) printf("%s\n", v->value.bool_val ? TRUE : FALSE);
            else           printf("%s" " ", v->value.bool_val ? TRUE : FALSE);
            break;
        }
        case VAL_STRING:
        {
            if(line_break) printf("%s\n", v->value.string_val);
            else           printf("%s" " ", v->value.string_val);
            break;
        }
        case VAL_NONE: default:
        {
            if(line_break) printf("%s\n", NONE);
            else           printf("%s" " ", NONE);
            break;
        }
    }
}

bool execute_print(print_stmt_t* print_stmt, int line)
{
    if (!print_stmt)
        process_error(EXECUTE_PRINT_func_PRINT_STMT_IS_NULL, line, false);

    if(!print_stmt -> expressions)
        process_error(EXECUTE_PRINT_func_EXPRS_IS_NULL, line, false);
    
    int exprs_count = print_stmt -> expr_count, i = 0;
    while(exprs_count && print_stmt->expressions[i])
    {
        value_t* v = eval(print_stmt -> expressions[i]);
        if(!v)
            process_error(EXECUTE_PRINT_func_PRINT_ARG_EVAL_ERROR,
                            line,
                             false);

        bool line_break = print_stmt -> expressions[i + 1] ? false : true;
        print_value(v, line_break);

        --exprs_count, ++i;
    }

    return true;
}

static value_t parse_input(const char *buf)
{
    value_t val;

    if (strcmp(buf, "true") == 0)
    {
        val.type = VAL_BOOL;
        val.value.bool_val = true;
        return val;
    }

    if (strcmp(buf, "false") == 0)
    {
        val.type = VAL_BOOL;
        val.value.bool_val = false;
        return val;
    }

    char *end;
    long long int_val = strtoll(buf, &end, 10);
    if (*end == '\0' && end != buf)
    {
        val.type = VAL_INT;
        val.value.int_val = (int64_t)int_val;
        return val;
    }

    double float_val = strtod(buf, &end);
    if (*end == '\0' && end != buf)
    {
        val.type = VAL_FLOAT;
        val.value.float_val = float_val;
        return val;
    }

    val.type = VAL_STRING;
    strncpy(val.value.string_val, buf, MAX_STR_SIZE - 1);
    val.value.string_val[MAX_STR_SIZE - 1] = '\0';
    return val;
}

bool execute_read(read_stmt_t* read_stmt, int line)
{
    if (!read_stmt)
        process_error(EXECUTE_READ_func_READ_STMT_IS_NULL, line, false);

    if (!global_map)
        global_map = hashmap_new();
    if (!global_map)
        process_error(HASHMAP_INIT_ERROR, line, false);

    char buf[MAX_STR_SIZE];
    if (!fgets(buf, MAX_STR_SIZE, stdin))
        process_error(EXECUTE_READ_func_READ_INPUT_ERROR, line, false);

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';

    value_t val = parse_input(buf);

    variable_t *var = hashmap_lookup(global_map, read_stmt->var_name);
    if (var)
        var->value = val;
    else
        hashmap_set(global_map, read_stmt->var_name, val, false);

    return true;
}

static value_t* interprete_condition(expr_node_t* expr, int line)
{
    value_t *val = eval(expr);
    if (!val)
        process_error(INTERPRET_COND_ERROR, line, false);

    if (val->type == VAL_BOOL)
        return val;

    val->value.bool_val = is_truthy(val);
    val->type = VAL_BOOL;
    return val;
}

bool execute_if(if_stmt_t* if_stmt, int line)
{
    if(!if_stmt)
        process_error(EXECUTE_IF_func_IF_STMT_IS_NULL,
                        line,
                         false);
                    
    if(!if_stmt->condition)
        process_error(EXECUTE_IF_func_IF_COND_IS_NULL,
                        line,
                         false);

    if(!if_stmt->then_branch)
        process_error(EXECUTE_IF_func_IF_THAN_IS_NULL,
                        line,
                         false);

    value_t *cond = interprete_condition(if_stmt->condition, line);
    if (!cond)
        return false;

    if (cond->value.bool_val)
    {
        if (!execute_block(if_stmt->then_branch, line))
            return false;
    }
    else if (if_stmt->else_branch)
    {
        if (!execute_block(if_stmt->else_branch, line))
            return false;
    }

    return true;
}

/* the virtual machine does not need any execution functions */
#if 0
bool execute_while(while_stmt_t* while_stmt, int line)
{
    if(!while_stmt)
        process_error(EXECUTE_IF_func_IF_STMT_IS_NULL,
                        line,
                         false);
        
    if(!while_stmt->condition)
        process_error(EXECUTE_WHILE_func_WHLE_COND_IS_NULL,
                        line,
                         false);

    if(!while_stmt->body)
    process_error(EXECUTE_WHILE_func_WHLE_BODY_IS_NULL,
                    line,
                     false);

    value_t *cond = NULL;

    while()

    do
    {
        cond = interprete_condition(while_stmt->condition, line);
        if (!cond)
            return false;
    }
}
#endif