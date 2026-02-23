#include "parser_api.h"
#include "parser.h"
#include "errs.h"
#include "lexer.h"
#include "mem.h"
#include "var.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define INC_POS(pos) (++(*pos))

static void add_stmt_to_block(stmt_node_t* stmt, block_stmt_t* block)
{
    if(block -> statements == NULL)
        /* firsts stmt in block */
        block -> statements = stmt;
    
    else
    {
        /* find the last stmt in block */
        stmt_node_t* curr = block -> statements;
        while(curr -> next)
            curr = curr -> next;

        curr -> next = stmt;
    }

    block -> stmt_count++;
}

block_stmt_t* parse_block(TOKEN_STREAM* stream, int* pos)
{
    /* pos point to the '{', so inc_pos */
    INC_POS(pos);

    block_stmt_t* block = pli_alloc(sizeof(block_stmt_t));
    block->statements = NULL;
    block->stmt_count = 0;

    /* parsing stmts until we meet rbrace '}' */
    while (*pos < (int)stream->count &&
           stream->tokens[*pos].type_token != sep_token_rbrace)
    {
        stmt_node_t* stmt = parse_statement(stream, pos);
        if (!stmt) return NULL;

        add_stmt_to_block(stmt, block);
    }

    if (*pos >= (int)stream->count)
        return error_handling(PARSE_BLOCK_func_MISSING_RBRACE, 
                              stream->tokens[*pos].line_number,
                               false);

    return block;
}

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
        return error_handling(CREATE_any_STMT_func_SECOND_TOKEN_NOT_LPAREN_ERROR,
                              stream->tokens[*pos - 1].line_number,
                               false);

    if_stmt_t* if_stmt = pli_alloc(sizeof(if_stmt_t));
    if(!if_stmt)
        return error_handling(MEMORY_ALLOCATION_ERROR,
                            stream->tokens[*pos].line_number,
                            false);

    INC_POS(pos);

    if_stmt -> condition = parse_expression(stream, pos);
    if(!if_stmt -> condition)
            return error_handling(CREATE_IF_STMT_func_CONDITION_PARSE_ERROR, 
                                  stream->tokens[*pos].line_number,
                                   false);

    /* now pos must point to the close paren ')' token */
    if(*pos >= (int)stream->count || stream->tokens[*pos].type_token != sep_token_rparen)
        return error_handling(CREATE_any_STMT_func_MISSING_RPAREN,
                              stream->tokens[*pos].line_number,
                               false);
    INC_POS(pos);

    /* now pos point to the opening curly brace '{' token */
    if(*pos >= (int)stream->count || stream->tokens[*pos].type_token != sep_token_lbrace)
        return error_handling(CREATE_any_STMT_func_MISSING_LBRACE,
                              stream->tokens[*pos].line_number,
                               false);
                            
    /* recurse parse subprogram in braces {} */
    block_stmt_t* then_block = parse_block(stream, pos);
    if(!then_block)
        return error_handling(CREATE_IF_STMT_func_THEN_BRANCH_PARSE_ERROR, 
                              stream->tokens[*pos].line_number,
                               false);

    stmt_node_t* then_stmt = create_stmt(then_block, STMT_BLOCK, stream->tokens[*pos].line_number);
    if(!then_stmt)
            return error_handling(PARSE_func_STMT_CREATION_ERROR,
                                  stream -> tokens[*pos].line_number,
                                   false);

    if_stmt -> then_branch = then_stmt;
    if_stmt -> else_branch = NULL;

    /* skip '}' */
    INC_POS(pos);

    /* optional: else branch parse */
    if (*pos < (int)stream->count &&
        stream->tokens[*pos].type_token == kw_token_else)
    {
        /* skip '{' */
        INC_POS(pos);

        if (stream->tokens[*pos].type_token != sep_token_lbrace)
            return error_handling(CREATE_any_STMT_func_MISSING_LBRACE,
                                  stream->tokens[*pos].line_number,
                                   false);

        block_stmt_t* else_block = parse_block(stream, pos);
        if (!else_block)
            return error_handling(CREATE_IF_STMT_func_ELSE_BRANCH_PARSE_ERROR,
                              stream -> tokens[*pos].line_number,
                               false);

        if_stmt->else_branch = create_stmt(else_block, STMT_BLOCK, stream->tokens[*pos].line_number);
        INC_POS(pos);
    }

    return if_stmt;
}

while_stmt_t* create_while_stmt(TOKEN_STREAM* stream, int* pos)
{
    if(stream->tokens[*pos].type_token != kw_token_while)
        return error_handling(CREATE_WHILE_STMT_func_FIRST_TOKEN_NOT_WHILE_ERROR,
                              stream->tokens[*pos].line_number,
                               false);

    INC_POS(pos);

    if(*pos >= (int)stream->count || stream->tokens[*pos].type_token != sep_token_lparen)
        return error_handling(CREATE_any_STMT_func_SECOND_TOKEN_NOT_LPAREN_ERROR,
                              stream->tokens[*pos - 1].line_number,
                               false);

    while_stmt_t* while_stmt = pli_alloc(sizeof(if_stmt_t));
    if(!while_stmt)
        return error_handling(MEMORY_ALLOCATION_ERROR,
                            stream->tokens[*pos].line_number,
                            false);

    INC_POS(pos);

    while_stmt -> condition = parse_expression(stream, pos);
    if(!while_stmt -> condition)
            return error_handling(CREATE_WHILE_STMT_func_CONDITION_PARSE_ERROR, 
                                  stream->tokens[*pos].line_number,
                                   false);

    /* now pos must point to the close paren ')' token */
    if(*pos >= (int)stream->count || stream->tokens[*pos].type_token != sep_token_rparen)
        return error_handling(CREATE_any_STMT_func_MISSING_RPAREN,
                              stream->tokens[*pos].line_number,
                               false);

    INC_POS(pos);

    /* now pos point to the opening curly brace '{' token */
    if(*pos >= (int)stream->count || stream->tokens[*pos].type_token != sep_token_lbrace)
        return error_handling(CREATE_any_STMT_func_MISSING_LBRACE,
                              stream->tokens[*pos].line_number,
                               false);

    /* recurse parse subprogram in braces {} */
    block_stmt_t* body = parse_block(stream, pos);
    if(!body)
        return error_handling(CRCREATE_WHILE_STMT_func_BODY_PARSE_ERROR, 
                              stream->tokens[*pos].line_number,
                               false);

    stmt_node_t* body_stmt = create_stmt(body, STMT_BLOCK, stream->tokens[*pos].line_number);
    if(!body_stmt)
            return error_handling(PARSE_func_STMT_CREATION_ERROR,
                                  stream -> tokens[*pos].line_number,
                                   false);

    while_stmt -> body = body_stmt;
    
    /* skip '}' */
    INC_POS(pos);

    return while_stmt;
}