#include "errs.h"
#include "mem.h"
#include "parser.h"
#include "executor.h"
#include "parser_api.h"
#include "var.h"

#include "string.h"

extern hashmap* global_map;

bool is_truthy(value_t *v)
{
    switch (v->type)
    {
        case VAL_BOOL:   return v->value.bool_val;
        case VAL_INT:    return v->value.int_val != 0;
        case VAL_FLOAT:  return v->value.float_val != 0.0;
        case VAL_STRING: return v->value.string_val[0] != '\0';
        case VAL_NONE:   return false;
    }
    return false;
}

static value_t* eval_unary(unary_expr_t* un, int line)
{
    value_t *operand = eval(un->operand);
    if (!operand)
        return error_handling(EVAL_UNARY_func_OPERAND_EVAL_ERROR,
                              line,
                              false);

    value_t *result = pli_alloc(sizeof(value_t));
    if (!result)
        return error_handling(MEMORY_ALLOCATION_ERROR, line, false);

    switch (un->op)
    {
        case OP_NEGATE:
        {
            if (operand->type == VAL_FLOAT)
            {
                result->type = VAL_FLOAT;
                result->value.float_val = -(operand->value.float_val);
            }
            else if (operand->type == VAL_INT)
            {
                result->type = VAL_INT;
                result->value.int_val = -(operand->value.int_val);
            }
            else
            {
                pli_free(result);
                return error_handling(EVAL_UNARY_func_NEGATE_NON_NUMERIC,
                                      line, false);
            }
            break;
        }

        case OP_NOT:
        {
            result->type = VAL_BOOL;
            result->value.bool_val = !is_truthy(operand);
            break;
        }

        default:
            pli_free(result);
            return error_handling(EVAL_UNARY_func_UNKNOWN_OPERATOR,
                                  line, false);
    }

    return result;
}

static value_t* eval_grouping(grouping_expr_t* group, int line)
{
    value_t *val = eval(group->expression);
    if (!val)
        return error_handling(EVAL_GROUPING_func_INNER_EVAL_ERROR,
                              line, false);
    return val;
}

static value_t* eval_binary(binary_expr_t* bin, int line)
{
    /* Short-circuit for logical operators — always return bool */
    if (bin->op == OP_AND || bin->op == OP_OR)
    {
        value_t *left = eval(bin->left);
        if (!left)
            return error_handling(EVAL_BINARY_func_LEFT_EVAL_ERROR, line, false);

        bool left_truthy = is_truthy(left);

        if ((bin->op == OP_AND && !left_truthy) || (bin->op == OP_OR && left_truthy))
        {
            value_t *result = pli_alloc(sizeof(value_t));
            if (!result)
                return error_handling(MEMORY_ALLOCATION_ERROR, line, false);
            result->type = VAL_BOOL;
            result->value.bool_val = left_truthy;
            return result;
        }

        value_t *right = eval(bin->right);
        if (!right)
            return error_handling(EVAL_BINARY_func_RIGHT_EVAL_ERROR, line, false);

        value_t *result = pli_alloc(sizeof(value_t));
        if (!result)
            return error_handling(MEMORY_ALLOCATION_ERROR, line, false);
        result->type = VAL_BOOL;
        result->value.bool_val = is_truthy(right);
        return result;
    }

    value_t *left  = eval(bin->left);
    if (!left)
        return error_handling(EVAL_BINARY_func_LEFT_EVAL_ERROR, line, false);

    value_t *right = eval(bin->right);
    if (!right)
        return error_handling(EVAL_BINARY_func_RIGHT_EVAL_ERROR, line, false);

    value_t *result = pli_alloc(sizeof(value_t));
    if (!result)
        return error_handling(MEMORY_ALLOCATION_ERROR, line, false);

    /* String concatenation: "abc" + "def" */
    if (bin->op == OP_ADD && left->type == VAL_STRING && right->type == VAL_STRING)
    {
        result->type = VAL_STRING;
        strncpy(result->value.string_val, left->value.string_val, MAX_STR_SIZE - 1);
        strncat(result->value.string_val, right->value.string_val,
                MAX_STR_SIZE - 1 - strlen(result->value.string_val));
        return result;
    }

    /* Equality: works on any matching types */
    if (bin->op == OP_EQUAL || bin->op == OP_NOT_EQUAL)
    {
        if (left->type != right->type)
        {
            result->type = VAL_BOOL;
            result->value.bool_val = (bin->op == OP_NOT_EQUAL);
            return result;
        }

        bool equal = false;
        switch (left->type)
        {
            case VAL_INT:    equal = left->value.int_val   == right->value.int_val;              break;
            case VAL_FLOAT:  equal = left->value.float_val == right->value.float_val;            break;
            case VAL_BOOL:   equal = left->value.bool_val  == right->value.bool_val;             break;
            case VAL_STRING: equal = strcmp(left->value.string_val, right->value.string_val) == 0; break;
            case VAL_NONE:   equal = true;                                                        break;
        }

        result->type = VAL_BOOL;
        result->value.bool_val = (bin->op == OP_EQUAL) ? equal : !equal;
        return result;
    }

    /* Arithmetic and comparison — numeric types only */
    if ((left->type != VAL_INT && left->type != VAL_FLOAT) ||
        (right->type != VAL_INT && right->type != VAL_FLOAT))
    {
        pli_free(result);
        return error_handling(EVAL_BINARY_func_UNSUPPORTED_OP_FOR_TYPE, line, false);
    }

    /* Promote both to double if either is float */
    bool is_float = (left->type == VAL_FLOAT || right->type == VAL_FLOAT);
    double lv = (left->type  == VAL_FLOAT) ? left->value.float_val  : (double)left->value.int_val;
    double rv = (right->type == VAL_FLOAT) ? right->value.float_val : (double)right->value.int_val;

    switch (bin->op)
    {
        /* Arithmetic */
        case OP_ADD:
            if (is_float) { result->type = VAL_FLOAT; result->value.float_val = lv + rv; }
            else          { result->type = VAL_INT;   result->value.int_val   = left->value.int_val + right->value.int_val; }
            break;
        case OP_SUBTRACT:
            if (is_float) { result->type = VAL_FLOAT; result->value.float_val = lv - rv; }
            else          { result->type = VAL_INT;   result->value.int_val   = left->value.int_val - right->value.int_val; }
            break;
        case OP_MULTIPLY:
            if (is_float) { result->type = VAL_FLOAT; result->value.float_val = lv * rv; }
            else          { result->type = VAL_INT;   result->value.int_val   = left->value.int_val * right->value.int_val; }
            break;
        case OP_DIVIDE:
            if (rv == 0.0)
            {
                pli_free(result);
                return error_handling(EVAL_BINARY_func_DIVISION_BY_ZERO, line, false);
            }
            result->type = VAL_FLOAT;
            result->value.float_val = lv / rv;
            break;
        case OP_MODULO:
            if (right->value.int_val == 0 && right->type == VAL_INT)
            {
                pli_free(result);
                return error_handling(EVAL_BINARY_func_DIVISION_BY_ZERO, line, false);
            }
            result->type = VAL_INT;
            result->value.int_val = left->value.int_val % right->value.int_val;
            break;

        /* Comparison */
        case OP_LESS:          result->type = VAL_BOOL; result->value.bool_val = lv <  rv; break;
        case OP_GREATER:       result->type = VAL_BOOL; result->value.bool_val = lv >  rv; break;
        case OP_LESS_EQUAL:    result->type = VAL_BOOL; result->value.bool_val = lv <= rv; break;
        case OP_GREATER_EQUAL: result->type = VAL_BOOL; result->value.bool_val = lv >= rv; break;

        default:
            pli_free(result);
            return error_handling(EVAL_BINARY_func_UNKNOWN_OPERATOR, line, false);
    }

    return result;
}

static value_t* eval_literal(literal_expr_t* l, int line)
{
    value_t* val = pli_alloc(sizeof(value_t));
    if(!val)
        return error_handling(MEMORY_ALLOCATION_ERROR,
                              line,
                               false);

    switch(l->type)
    {
        case LIT_BOOLEAN:
            val->type = VAL_BOOL;
            val->value.bool_val = l->value.boolean;
            break;
        case LIT_NUMBER:
            val->type = VAL_FLOAT;
            val->value.float_val = l->value.number;
            break;
        case LIT_STRING:
            val->type = VAL_STRING;
            strncpy(val->value.string_val, l->value.string, MAX_STR_SIZE);
            break;
        default:
            return error_handling(EVAL_LITERAL_func_UNDEFIED_LIT_TYPE,
                                  line,
                                   false);
    }

    return val;
}

static value_t* eval_var(variable_t* v, int line)
{
    value_t* val = pli_alloc(sizeof(value_t));
    if(!val)
        return error_handling(MEMORY_ALLOCATION_ERROR,
                                line,
                                false);
    
    switch (v->value.type)
    {
        case VAL_BOOL:
        {
            val->type = VAL_BOOL;
            val->value.bool_val = v->value.value.bool_val;
            break;
        }
        case VAL_INT:
        {
            val->type = VAL_INT;
            val->value.int_val = v->value.value.int_val;
            break;
        }
        case VAL_FLOAT:
        {
            val->type = VAL_FLOAT;
            val->value.float_val = v->value.value.float_val;
            break;
        }
        case VAL_STRING:
        {
            val->type = VAL_STRING;
            strncpy(val->value.string_val, v->value.value.string_val, MAX_STR_SIZE);
            break;
        }
        case VAL_NONE: default:
            return error_handling(EVAL_VAL_func_UNDEFINED_VAR_TYPE,
                                  line,
                                   false);
    }

    return val;
}

value_t* eval(expr_node_t* expr)
{
    switch (expr -> type)
    {
        case EXPR_VARIABLE:
        {
            variable_t* v = NULL;
            if(!global_map)
                return error_handling(EVAL_func_NON_EXIST_VAR_REFERENCE, 
                    expr -> line,
                    false);

            if(!(v = hashmap_lookup(global_map, expr ->expr.variable->name)))
                return error_handling(EVAL_func_NON_EXIST_VAR_REFERENCE, 
                                      expr -> line,
                                       false);

            return eval_var(v, expr->line);
        }

        case EXPR_LITERAL:  return eval_literal(expr->expr.literal, expr->line);
        case EXPR_UNARY:    return eval_unary(expr->expr.unary, expr->line);
        case EXPR_BINARY:   return eval_binary(expr->expr.binary, expr->line);
        case EXPR_GROUPING: return eval_grouping(expr->expr.grouping, expr->line);

        default: 
            return error_handling(EVAL_funv_UNDEFINED_EXPR_TYPE,
                                  expr -> line,
                                   false);
    }
}