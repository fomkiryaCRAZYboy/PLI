#ifndef PARSER_API_H
#define PARSER_API_H

#include "lexer.h"
#include "parser.h"

base_node_t* parsing(TOKEN_STREAM* stream);/* main parsing function, that return nodes array
                                      the parsing function takes a stream of tokens as an argument 
                                      */
                                     
/* print operation helper funcs */
int print_statement_handling(TOKEN* tokens, int* iter);
void print_add_arg(TOKEN token, value_t* args, size_t* args_count);

bool is_var_exists(char* var_name);

#endif /* PARSER_API_H */