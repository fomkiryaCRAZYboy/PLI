#include "parser_api.h"
#include "parser.h"
#include "errs.h"
#include "lexer.h"
#include "mem.h"
#include "var.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * AST node creation helpers
 * ============================================================================ */

static expr_node_t* create_binary_expr(binary_op_t op, expr_node_t* left, expr_node_t* right)
{
    binary_expr_t* binary = pli_alloc(sizeof(binary_expr_t));
    if(!binary)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    binary->op    = op;
    binary->left  = left;
    binary->right = right;

    expr_node_t* node = pli_alloc(sizeof(expr_node_t));
    if(!node)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    node->type        = EXPR_BINARY;
    node->expr.binary = binary;
    return node;
}

static expr_node_t* create_unary_expr(unary_op_t op, expr_node_t* operand)
{
    unary_expr_t* unary = pli_alloc(sizeof(unary_expr_t));
    if(!unary)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    unary->op      = op;
    unary->operand = operand;

    expr_node_t* node = pli_alloc(sizeof(expr_node_t));
    if(!node)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    node->type       = EXPR_UNARY;
    node->expr.unary = unary;
    return node;
}

static expr_node_t* create_literal_number(double value)
{
    literal_expr_t* lit = pli_alloc(sizeof(literal_expr_t));
    if(!lit)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    lit->type         = LIT_NUMBER;
    lit->value.number = value;

    expr_node_t* node = pli_alloc(sizeof(expr_node_t));
    if(!node)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    node->type         = EXPR_LITERAL;
    node->expr.literal = lit;
    return node;
}

static expr_node_t* create_literal_string(const char* str)
{
    literal_expr_t* lit = pli_alloc(sizeof(literal_expr_t));
    if(!lit)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    lit->type = LIT_STRING;
    strncpy(lit->value.string, str, MAX_STR_SIZE);

    expr_node_t* node = pli_alloc(sizeof(expr_node_t));
    if(!node)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    node->type         = EXPR_LITERAL;
    node->expr.literal = lit;
    return node;
}

static expr_node_t* create_literal_bool(bool value)
{
    literal_expr_t* lit = pli_alloc(sizeof(literal_expr_t));
    if(!lit)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    lit->type          = LIT_BOOLEAN;
    lit->value.boolean = value;

    expr_node_t* node = pli_alloc(sizeof(expr_node_t));
    if(!node)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    node->type         = EXPR_LITERAL;
    node->expr.literal = lit;
    return node;
}

static expr_node_t* create_variable_expr(const char* name)
{
    variable_expr_t* var = pli_alloc(sizeof(variable_expr_t));
    if(!var)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    strncpy(var->name, name, MAX_VAR_SIZE);

    expr_node_t* node = pli_alloc(sizeof(expr_node_t));
    if(!node)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    node->type          = EXPR_VARIABLE;
    node->expr.variable = var;
    return node;
}

static expr_node_t* create_grouping_expr(expr_node_t* inner)
{
    grouping_expr_t* group = pli_alloc(sizeof(grouping_expr_t));
    if(!group)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    group->expression = inner;

    expr_node_t* node = pli_alloc(sizeof(expr_node_t));
    if(!node)
        return error_handling(MEMORY_ALLOCATION_ERROR, -1, false);

    node->type          = EXPR_GROUPING;
    node->expr.grouping = group;
    return node;
}

/* ============================================================================
 * Helpers
 * ============================================================================ */

static inline bool at_end(TOKEN_STREAM* stream, int pos)
{
    return pos >= (int)stream->count;
}

static inline TOKEN_TYPE peek(TOKEN_STREAM* stream, int pos)
{
    return stream->tokens[pos].type_token;
}

static inline int current_line(TOKEN_STREAM* stream, int pos)
{
    if(pos < (int)stream->count)
        return stream->tokens[pos].line_number;
    return stream->tokens[stream->count - 1].line_number;
}

/* ============================================================================
 * Recursive descent: forward declarations
 * ============================================================================ */

static expr_node_t* parse_comparison(TOKEN_STREAM* stream, int* pos);
static expr_node_t* parse_term(TOKEN_STREAM* stream, int* pos);
static expr_node_t* parse_factor(TOKEN_STREAM* stream, int* pos);
static expr_node_t* parse_unary(TOKEN_STREAM* stream, int* pos);
static expr_node_t* parse_primary(TOKEN_STREAM* stream, int* pos);

/* ============================================================================
 * primary = number | string | boolean | identifier | "(" expression ")"
 * ============================================================================ */

static expr_node_t* parse_primary(TOKEN_STREAM* stream, int* pos)
{
    if(at_end(stream, *pos))
        return error_handling(PARSE_EXPR_UNEXPECTED_TOKEN, current_line(stream, *pos), false);

    TOKEN* token = &stream->tokens[*pos];

    switch(token->type_token)
    {
        case num_token:
        {
            double val = atof(token->text_token.num_text);
            ++(*pos);
            return create_literal_number(val);
        }

        case str_token:
        {
            ++(*pos);
            return create_literal_string(token->text_token.text);
        }

        case bool_token_t:
            ++(*pos);
            return create_literal_bool(true);

        case bool_token_f:
            ++(*pos);
            return create_literal_bool(false);

        case iden_token:
        {
            ++(*pos);
            return create_variable_expr(token->text_token.text);
        }

        case sep_token_lparen:
        {
            ++(*pos);
            expr_node_t* inner = parse_expression(stream, pos);
            if(!inner)
                return NULL;

            if(at_end(stream, *pos) || peek(stream, *pos) != sep_token_rparen)
                return error_handling(PARSE_EXPR_MISSING_RPAREN, current_line(stream, *pos), false);

            ++(*pos);
            return create_grouping_expr(inner);
        }

        default:
            return error_handling(PARSE_EXPR_UNEXPECTED_TOKEN, token->line_number, false);
    }
}

/* ============================================================================
 * unary = ["-" | "not"] primary
 * ============================================================================ */

static expr_node_t* parse_unary(TOKEN_STREAM* stream, int* pos)
{
    if(!at_end(stream, *pos))
    {
        TOKEN_TYPE t = peek(stream, *pos);

        if(t == math_op_token_minus)
        {
            ++(*pos);
            expr_node_t* operand = parse_unary(stream, pos);
            if(!operand)
                return NULL;
            return create_unary_expr(OP_NEGATE, operand);
        }

        if(t == kw_token_not)
        {
            ++(*pos);
            expr_node_t* operand = parse_unary(stream, pos);
            if(!operand)
                return NULL;
            return create_unary_expr(OP_NOT, operand);
        }
    }

    return parse_primary(stream, pos);
}

/* ============================================================================
 * factor = unary { ("*" | "/" | "%") unary }
 * ============================================================================ */

static expr_node_t* parse_factor(TOKEN_STREAM* stream, int* pos)
{
    expr_node_t* left = parse_unary(stream, pos);
    if(!left) return NULL;

    while(!at_end(stream, *pos))
    {
        TOKEN_TYPE t = peek(stream, *pos);
        binary_op_t op;

        if(t == math_op_token_mult)         op = OP_MULTIPLY;
        else if(t == math_op_token_div)     op = OP_DIVIDE;
        else if(t == math_op_token_percent) op = OP_MODULO;
        else break;

        ++(*pos);
        expr_node_t* right = parse_unary(stream, pos);
        if(!right)
            return NULL;

        left = create_binary_expr(op, left, right);
        if(!left) return NULL;
    }

    return left;
}

/* ============================================================================
 * term = factor { ("+" | "-") factor }
 * ============================================================================ */

static expr_node_t* parse_term(TOKEN_STREAM* stream, int* pos)
{
    expr_node_t* left = parse_factor(stream, pos);
    if(!left) return NULL;

    while(!at_end(stream, *pos))
    {
        TOKEN_TYPE t = peek(stream, *pos);
        binary_op_t op;

        if(t == math_op_token_plus)       op = OP_ADD;
        else if(t == math_op_token_minus) op = OP_SUBTRACT;
        else break;

        ++(*pos);
        expr_node_t* right = parse_factor(stream, pos);
        if(!right)
            return NULL;

        left = create_binary_expr(op, left, right);
        if(!left) return NULL;
    }

    return left;
}

/* ============================================================================
 * comparison = term { ("==" | "!=" | "<" | ">" | "<=" | ">=") term }
 * ============================================================================ */

static expr_node_t* parse_comparison(TOKEN_STREAM* stream, int* pos)
{
    expr_node_t* left = parse_term(stream, pos);
    if(!left) return NULL;

    while(!at_end(stream, *pos))
    {
        TOKEN_TYPE t = peek(stream, *pos);
        binary_op_t op;

        if(t == comp_op_token_equal)           op = OP_EQUAL;
        else if(t == comp_op_token_not_equal)  op = OP_NOT_EQUAL;
        else if(t == comp_op_token_less)       op = OP_LESS;
        else if(t == comp_op_token_more)       op = OP_GREATER;
        else if(t == comp_op_token_less_equal) op = OP_LESS_EQUAL;
        else if(t == comp_op_token_more_equal) op = OP_GREATER_EQUAL;
        else break;

        ++(*pos);
        expr_node_t* right = parse_term(stream, pos);
        if(!right)
            return NULL;

        left = create_binary_expr(op, left, right);
        if(!left) return NULL;
    }

    return left;
}

/* ============================================================================
 * expression = comparison { ("and" | "or") comparison }
 * ============================================================================ */

expr_node_t* parse_expression(TOKEN_STREAM* stream, int* pos)
{
    expr_node_t* left = parse_comparison(stream, pos);
    if(!left) return NULL;

    while(!at_end(stream, *pos))
    {
        TOKEN_TYPE t = peek(stream, *pos);
        binary_op_t op;

        if(t == kw_token_and)      op = OP_AND;
        else if(t == kw_token_or)  op = OP_OR;
        else break;

        ++(*pos);
        expr_node_t* right = parse_comparison(stream, pos);
        if(!right)
            return NULL;

        left = create_binary_expr(op, left, right);
        if(!left) return NULL;
    }

    return left;
}
