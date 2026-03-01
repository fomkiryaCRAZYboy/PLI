#include "executor.h"
#include "errs.h"
#include "parser.h"
#include "parser_api.h"
#include <stdbool.h>

#define safe_line(s) ((s) ? (s)->line : 0)

static bool execute_stmt(stmt_node_t *stmt)
{
    int line = safe_line(stmt);

    switch (stmt->type)
    {
        case STMT_VAR_DECL:
            return execute_var_decl(stmt->as.var_decl, line);
        case STMT_ASSIGNMENT:
            return execute_assignment(stmt->as.assignment, line);
        case STMT_IF:
            return execute_if(stmt->as.if_stmt, line);
        case STMT_WHILE:
            /* TODO */
            return true;
        case STMT_PRINT:
            return execute_print(stmt->as.print_stmt, line);
        case STMT_READ:
            return execute_read(stmt->as.read_stmt, line);
        case STMT_BLOCK:
            return execute_block(stmt->as.block->statements, line);
        default:
            add_err_code(EXECUTE_func_UNDEFINED_STMT_TYPE, line, false);
            return false;
    }
}

bool execute_block(stmt_node_t *stmts, int line)
{
    stmt_node_t *curr = stmts;

    while (curr)
    {
        if (!execute_stmt(curr))
            return false;
        curr = curr->next;
    }

    return true;
}

bool execute(program_t *program)
{
    if (!program || !program->statements)
        return false;

    return execute_block(program->statements, 0);
}
