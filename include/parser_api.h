#ifndef PARSER_API_H
#define PARSER_API_H

#include "parser.h"
#include "lexer.h"

program_t* parse(TOKEN_STREAM* stream);
stmt_node_t* create_stmt(void* any_stmt, stmt_type_t type, int line_num);
var_decl_stmt_t* create_var_decl_stmt(TOKEN_STREAM* stream, int* pos);
assignment_stmt_t* create_assignment_stmt(TOKEN_STREAM* stream, int* pos);
if_stmt_t* create_if_stmt(TOKEN_STREAM* stream, int* pos);
while_stmt_t* create_while_stmt(TOKEN_STREAM* stream, int* pos);
expr_node_t* parse_expression(TOKEN_STREAM* stream, int* pos);
stmt_node_t* parse_statement(TOKEN_STREAM* stream, int* pos);

/* helpers */
void* error_handling(int err_code, int line_num, bool is_warn);

/* DEBUG */
void print_ast(program_t* program);

#endif /* PARSER_API_H */