#include "lexer.h"
#include "stdio.h"

/* main parse function. it has one parameter - tokens stream returned by tokenize() func. */
int parse(TOKEN_STREAM* stream)
{
    for (int i = 0; i < stream->count; ++i)
    {
        switch (stream ->tokens[i].type_token)
        {
            case kw_token_var: printf("varrf\n"); return 0;
            default: return 1;
        }
    }
}