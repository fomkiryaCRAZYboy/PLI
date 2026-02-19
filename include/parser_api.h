#ifndef PARSER_API_H
#define PARSER_API_H

#include "parser.h"
#include "lexer.h"

program_t* parse(TOKEN_STREAM* stream);
var_decl_stmt_t* create_var_decl_stmt(TOKEN_STREAM* stream, int* pos);
assignment_stmt_t* create_assignment_stmt(TOKEN_STREAM* stream, int* pos);
if_stmt_t* create_if_stmt(TOKEN_STREAM* stream, int* pos);
expr_node_t* parse_expression(TOKEN_STREAM* stream, int* pos);

/* helpers */
void* error_handling(int err_code, int line_num, bool is_warn);

#endif /* PARSER_API_H */