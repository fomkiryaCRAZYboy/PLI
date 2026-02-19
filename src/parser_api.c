#include "parser_api.h"
#include "parser.h"
#include "errs.h"
#include "lexer.h"
#include "mem.h"
#include "var.h"

#include <stdbool.h>
#include <string.h>

#define INC_POS(pos) (++(*pos))

void* error_handling(int err_code, int line_num, bool is_warn)
{
    add_err_code(err_code, line_num, is_warn);
    return NULL;
}

var_decl_stmt_t* create_var_decl_stmt(TOKEN_STREAM* stream, int* pos)
{
    if(stream->tokens[*pos].type_token != kw_token_var)
        return error_handling(CREATE_VAR_DECL_STMT_func_FIRST_TOKEN_NOT_VAR_ERROR,
                              stream->tokens[*pos].line_number,
                               false);

    INC_POS(pos);

    if(*pos >= (int)stream->count || stream->tokens[*pos].type_token != iden_token)
        return error_handling(CREATE_VAR_DECL_STMT_func_SECOND_TOKEN_NOT_IDEN_ERROR,
                              stream->tokens[*pos - 1].line_number,
                               false);
    
    var_decl_stmt_t* var_decl_stmt = pli_alloc(sizeof(var_decl_stmt_t));
    if(!var_decl_stmt)
        return error_handling(MEMORY_ALLOCATION_ERROR,
                              stream->tokens[*pos].line_number,
                               false);

    strncpy(var_decl_stmt->var_name, stream->tokens[*pos].text_token.text, MAX_VAR_SIZE);

    INC_POS(pos);

    if(*pos >= (int)stream->count || stream->tokens[*pos].type_token != assing_op_token)
        return error_handling(CREATE_VAR_DECL_STMT_func_THIRD_TOKEN_NOT_ASSIGN_ERROR,
                              stream->tokens[*pos - 1].line_number, 
                               false);

    INC_POS(pos);

    var_decl_stmt->initializer = parse_expression(stream, pos);
    if(!var_decl_stmt->initializer)
        return error_handling(CREATE_VAR_DECL_STMT_func_INITIALIZER_PARSE_ERROR,
                              stream->tokens[*pos - 1].line_number,
                               false);

    return var_decl_stmt;
}

assignment_stmt_t* create_assignment_stmt(TOKEN_STREAM* stream, int* pos)
{
    if(stream->tokens[*pos].type_token != iden_token)
        return error_handling(CREATE_ASSIGNMENT_STMT_func_FIRST_TOKEN_NOT_IDEN_ERROR,
                              stream->tokens[*pos].line_number,
                               false);

    int iden_pos = *pos;
    INC_POS(pos);

    if(*pos >= (int)stream->count || stream->tokens[*pos].type_token != assing_op_token)
        return error_handling(CREATE_ASSIGNMENT_STMT_func_SECOND_TOKEN_NOT_ASSIGN_ERROR,
                              stream->tokens[*pos - 1].line_number,
                               false);

    assignment_stmt_t* assignment_stmt = pli_alloc(sizeof(assignment_stmt_t));
    if(!assignment_stmt)
        return error_handling(MEMORY_ALLOCATION_ERROR,
                              stream->tokens[*pos].line_number,
                               false);

    strncpy(assignment_stmt->var_name,
            stream->tokens[iden_pos].text_token.text,
            MAX_VAR_SIZE);

    INC_POS(pos);

    assignment_stmt -> value = parse_expression(stream, pos);
    if(!assignment_stmt -> value)
        return error_handling(CREATE_ASSIGNMENT_STMT_func_ASSIGNMENT_VALUE_PARSE_ERROR, 
                              stream->tokens[*pos].line_number,
                              false);

    return assignment_stmt;
}

if_stmt_t* create_if_stmt(TOKEN_STREAM* stream, int* pos)
{
    if(stream->tokens[*pos].type_token != kw_token_if)
        return error_handling(CREATE_IF_STMT_func_FIRST_TOKEN_NOT_IF_ERROR,
                              stream->tokens[*pos].line_number,
                               false);

    INC_POS(pos);

    if(*pos >= (int)stream->count || stream->tokens[*pos].type_token != sep_token_lparen)
        return error_handling(CREATE_IF_STMT_func_SECOND_TOKEN_NOT_LPAREN_ERROR,
                              stream->tokens[*pos - 1].line_number,
                               false);

    if_stmt_t* if_stmt = pli_alloc(sizeof(if_stmt_t));
    if(!if_stmt)
        return error_handling(MEMORY_ALLOCATION_ERROR,
                            stream->tokens[*pos].line_number,
                            false);
    //if_stmt -> condition = parse_expression(TOKEN_STREAM *stream, int *pos)
            
    return if_stmt;
}