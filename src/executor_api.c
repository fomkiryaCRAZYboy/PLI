#include "executor.h"
#include "parser.h"
#include "errs.h"
#include "parser_api.h"
#include "var.h"

void* execute_var_decl(var_decl_stmt_t* var_decl_stmt, int line)
{
    if(!var_decl_stmt)
        return error_handling(EXECUTE_VAR_DECL_func_VAR_DECL_STMT_IS_NULL,
                              line,
                               false);

    if(!var_decl_stmt->initializer)
        return error_handling(EXECUTE_VAR_DECL_func_INITIALIZER_IS_NULL,
                              line,
                               false);

    if(var_hash_lookup(var_decl_stmt -> var_name))
        return error_handling(EXECUTE_VAR_DECL_func_REDECLARATION_ERROR,
                                line,
                                false);

    
}
