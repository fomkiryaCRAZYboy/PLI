#ifndef PARSER_API_H
#define PARSER_API_H

#include "parser.h"

#define MAX_GROUPING_EXPR_TOKENS_COUNT  250

#include "lexer.h"

program_t* parse(TOKEN_STREAM* stream);
expr_node_t* create_expr_node(TOKEN_STREAM* stream, int position);
var_decl_stmt_t* create_var_decl_stmt(TOKEN_STREAM* stream, int position);
void parse_expression();

/* helpers */
void* error_handling(int err_code, int line_num, bool is_warn);

#endif /* PARSER_API_H */