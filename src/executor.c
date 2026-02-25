#include "executor.h"
#include "errs.h"
#include "parser.h"
#include "parser_api.h"
#include <stdbool.h>

static inline int safe_line(stmt_node_t* s) { return s ? s->line : 0; }

void* execute(program_t *program)
{
    if(!program || !program -> statements)
        return NULL;

    stmt_node_t* curr_stmt = program -> statements;

    while(curr_stmt)
    {
        switch (curr_stmt -> type) {
            case STMT_VAR_DECL:
                execute_var_decl(curr_stmt ->as.var_decl, safe_line(curr_stmt));
                break;
            case STMT_ASSIGNMENT:
                break;
            case STMT_IF:
                break;
            case STMT_WHILE:
                break;
            case STMT_PRINT:
                break;
            case STMT_READ:
                break;
            case STMT_BLOCK:
                break;

            default:
                return error_handling(EXECUTE_func_UNDEFINED_STMT_TYPE,
                                      safe_line(curr_stmt),
                                       false);
                
        }
        curr_stmt = curr_stmt -> next;
    }

    return NULL;
}