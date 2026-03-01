#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"
#include <stdbool.h>

bool     execute(program_t* program);
bool     execute_var_decl(var_decl_stmt_t* var_decl_stmt, int line);
bool     execute_assignment(assignment_stmt_t* assign_stmt, int line);
bool     execute_print(print_stmt_t* print_stmt, int line);
bool     execute_read(read_stmt_t* read_stmt, int line);
bool     execute_if(if_stmt_t* if_stmt, int line);
bool     execute_block(stmt_node_t* stmts, int line);
value_t* eval(expr_node_t* expr);
bool     is_truthy(value_t *v);

#endif /* EXECUTOR_H */
