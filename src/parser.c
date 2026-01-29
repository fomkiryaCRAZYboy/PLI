#include "parser_api.h"
#include "lexer.h"
#include "errs.h"
#include "parser.h"

/* root of AST-Tree */
ast_node_t* ast_tree = NULL ;

int parsing(TOKEN_STREAM* stream)
{
    int iter;

    /* main parsing cycle, it iterates over the program's token stream */
    for(iter = 0; iter < stream -> count; ++iter)
    {
        if(stream -> tokens[iter].type_token == kw_token_print)
        {
            /* add to AST-Tree print_statement node */
            print_statement_handling(stream -> tokens, &iter);
        }
    }
}