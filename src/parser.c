#include "parser_api.h"
#include "lexer.h"
#include "errs.h"

base_node_t* parsing(TOKEN_STREAM* stream)
{
    int iter;

    /* main parsing cycle, it sorts through tokens stream */
    for(iter = 0; iter < stream -> count; ++iter)
    {
        if(stream -> tokens[iter].type_token == kw_token_print)
        {
            print_statement_handling(stream -> tokens, &iter);
        }
    }
}