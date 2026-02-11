#include "parser_api.h"
#include "parser.h"
#include "errs.h"
#include "lexer.h"
#include "mem.h"
#include "var.h"

#include <stdbool.h>
#include <string.h>

void* error_handling(int err_code, int line_num, bool is_warn)
{
    add_err_code(err_code, line_num, is_warn);
    return NULL;
}

static bool is_valid_expr(TOKEN_STREAM* stream, int position)
{
    /* if line num increases, there is an error */
    int curr_line = stream ->tokens[position].line_number;
    int count = position

    while()

    return true;
}

#if 0
static void copy_token(TOKEN* src_token, TOKEN* dest_token)
{
    dest_token -> type_token = src_token -> type_token;
    dest_token -> line_number = src_token -> line_number;
    dest_token -> text_token = src_token -> text_token;
    return ;
}
#endif

var_decl_stmt_t* create_var_decl_stmt(TOKEN_STREAM* stream, int position)
{
    /* the first token must be 'kw_token_var', second token must be 'iden_token', third token must be 'assing_op_token'*/
    if(stream->tokens[position].type_token != kw_token_var)
        /* error, the first token is not 'kw_token_var' */
        return error_handling(CREATE_VAR_DECL_STMT_func_FIRST_TOKEN_NOT_VAR_ERROR, stream->tokens[position].line_number, false);

    if(stream->tokens[++position].type_token != iden_token)
        /* error, second token is not 'iden_token' */
        return error_handling(CREATE_VAR_DECL_STMT_func_SECOND_TOKEN_NOT_IDEN_ERROR, stream->tokens[position].line_number, false);
    
    var_decl_stmt_t* var_decl_stmt = pli_alloc(sizeof(var_decl_stmt_t));
    if(!var_decl_stmt)
        return error_handling(MEMORY_ALLOCATION_ERROR, stream->tokens[position].line_number, false);

    strncpy(var_decl_stmt->var_name, stream->tokens[position].text_token.text, MAX_VAR_SIZE);

    if(stream->tokens[++position].type_token != assing_op_token)
        /* error, third token is not 'assing_op_token' */
        return error_handling(CREATE_VAR_DECL_STMT_func_THIRD_TOKEN_NOT_ASSIGN_ERROR, stream->tokens[position].line_number, false);

    var_decl_stmt -> initializer = create_expr_node(stream, position);
    if(!var_decl_stmt -> initializer)
        return error_handling(CREATE_VAR_DECL_STMT_func_INITIALIZER_PARSE_ERROR, stream->tokens[position].line_number, false);

    return var_decl_stmt;
}

/*  */
expr_node_t* create_expr_node(TOKEN_STREAM* stream, int position)
{
    int token_count = 0;
    
    /* Validate expression and get token count */
    if (!is_valid_expr(stream, position, &token_count))
        return error_handling(CREATE_EXPR_NODE_func_INVALID_EXPRESSION, 
                            stream->tokens[position].line_number, false);
    
    /* TODO: реализовать парсинг выражения используя token_count токенов */
    parse_expression();
    
    expr_node_t* expr = pli_alloc(sizeof(expr_node_t));
    if (!expr)
        return error_handling(MEMORY_ALLOCATION_ERROR, 
                            stream->tokens[position].line_number, false);
    
    return expr;
}