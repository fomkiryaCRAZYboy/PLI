#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

void* execute(program_t* program);
void* execute_var_decl(var_decl_stmt_t* var_decl_stmt, int line);

#endif /* EXECUTOR_H */