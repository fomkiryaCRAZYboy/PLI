#include "errs.h"
#include "lexer.h"
#include "mem.h"
#include "parser_api.h"
#include "parser.h"

#include <stdbool.h>
#include <stdio.h>

stmt_node_t* parse_statement(TOKEN_STREAM* stream, int* pos)
{
    int stmt_line = stream->tokens[*pos].line_number;

    switch (stream->tokens[*pos].type_token)
    {
        case kw_token_var:
        {
            var_decl_stmt_t* vd = create_var_decl_stmt(stream, pos);
            if (!vd) 
                return error_handling(PARSE_func_CREATE_VAR_DECL_STMT_ERROR,
                                      stmt_line,
                                       false);

            return create_stmt(vd, STMT_VAR_DECL, stmt_line);
        }
        case iden_token:
        {
            assignment_stmt_t* as = create_assignment_stmt(stream, pos);
            if (!as) 
                return error_handling(PARSE_func_CREATE_ASSIGNMENT_STMT_ERROR,
                                      stmt_line,
                                       false);

            return create_stmt(as, STMT_ASSIGNMENT, stmt_line);
        }
        case kw_token_if:
        {
            if_stmt_t* ifs = create_if_stmt(stream, pos);
            if (!ifs) 
                return error_handling(PARSE_func_CREATE_IF_STMT_ERROR,
                                      stmt_line,
                                       false);

            return create_stmt(ifs, STMT_IF, stmt_line);
        }
        case kw_token_while:
        {
            while_stmt_t* whs = create_while_stmt(stream, pos);
            if(!whs)
                return error_handling(PARSE_func_CREATE_WHILE_STMT_ERROR, 
                                      stmt_line,
                                      false);

            return create_stmt(whs, STMT_WHILE, stmt_line);
        }

        /* print, read */
        default:
            return NULL;
    }
}

/* add new stmt to stmts_list */
static void add_stmt_to_list(stmt_node_t* stmt, program_t* program)
{   
    if(program -> statements == NULL)
        /* first statement in list */
        program -> statements = stmt;
    else
    {
        /* find the last stmt in list */
        stmt_node_t* curr = program -> statements;
        while(curr -> next)
            curr = curr -> next;

        curr -> next = stmt;
    }

    program -> stmt_count++;
}

/*  */
stmt_node_t* create_stmt(void* any_stmt, stmt_type_t type, int line_num)
{
    stmt_node_t* stmt = pli_alloc(sizeof(stmt_node_t));
    if(!stmt)
    {
        add_err_code(MEMORY_ALLOCATION_ERROR, -1, false);
        return NULL;
    }

    stmt -> type = type;

    switch (type) 
    {
        case STMT_VAR_DECL:
            stmt -> as.var_decl = (var_decl_stmt_t*)any_stmt; 
            break;
        case STMT_ASSIGNMENT:
            stmt -> as.assignment = (assignment_stmt_t*)any_stmt;
            break;
        case STMT_BLOCK:
            stmt -> as.block = (block_stmt_t*)any_stmt;
            break;
        case STMT_IF:
            stmt -> as.if_stmt = (if_stmt_t*)any_stmt;
            break;
        case STMT_PRINT:
            stmt -> as.print_stmt = (print_stmt_t*)any_stmt;
            break;
        case STMT_READ:
            stmt -> as.read_stmt = (read_stmt_t*)any_stmt;
            break;
        case STMT_WHILE:
            stmt -> as.while_stmt = (while_stmt_t*)any_stmt;
            break;
        
        default: 
            goto err;    
    }

    stmt -> next = NULL;
    return stmt;

err:
pli_free(stmt);
add_err_code(CREATE_STMT_func_UNKNOWN_STMT_TYPE, line_num, false);
return NULL;
}

/* main parse function. it has one parameter - tokens stream returned by tokenize() func. */
program_t* parse(TOKEN_STREAM* stream)
{
    program_t* program = pli_alloc(sizeof(program_t));
    if(!program)
    {
        add_err_code(MEMORY_ALLOCATION_ERROR, -1, false);
        return NULL;
    }

    program -> stmt_count = 0;
    program -> statements = NULL;

    stmt_node_t* stmt = NULL;
    int pos = 0, line = 0;
    while(pos < (int)stream->count)
    {
        stmt = parse_statement(stream, &pos);
        line = stream->tokens[pos].line_number; 

        if(!stmt)
            return error_handling(PARSE_func_STMT_CREATION_ERROR,
                                  line,
                                   false);
        else
            add_stmt_to_list(stmt, program);
    }    

    return program;
}