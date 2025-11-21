#include "parser_api.h"
#include "mem.h"

int print_add_arg(TOKEN token, value_t* args, size_t* args_count)
{
    switch (token.type_token)
        {
            case kw_token_var:
                /* check variable existence */
                if(!is_var_exists(token.text_token.text))
                {
                    add_err_code(PRINT_ADD_ARG_func_VARIABLE_NOT_EXIST_ERROR, token.line_number, false);
                    return PRINT_ADD_ARG_func_VARIABLE_NOT_EXIST_ERROR;
                }

                args[*args_count].

                break;

            case kw_token_not:

            case iden_token:
            case str_token:
            case num_token:
            
            case bool_token_f:
            case bool_token_t:

            case math_op_token_plus:
            case math_op_token_minus:
            
            /* Invalid argument error processing */
            default:
                add_err_code(PARSING_func_INVALID_ARG_PRINT_ST_ERROR, token.line_number, false);
                return PARSING_func_INVALID_ARG_PRINT_ST_ERROR;
        }
}

static int collect_args(TOKEN* args, int* iter)
{
    /* 1 - one paren is not closed, 2 - two parens are not closed, ... , 0 - all paren are closed */
    int paren_lvl = 1;

    TOKEN

    /* tokens are collected until a closing parenthesis is found */
    while(paren_lvl)
    {

    }
}

int print_statement_handling(TOKEN* tokens, int* iter)
{
    value_t* args = NULL;
    size_t   args_count = 0;

    ++(*iter);

    /* missing lparen error processing */
    if(tokens[*iter].type_token != sep_token_lparen)
    {
        add_err_code(PARSING_func_MISSING_LPAREN_PRINT_ST_ERROR, tokens[*iter].line_number , false);
        return PARSING_func_MISSING_LPAREN_PRINT_ST_ERROR;
    }
    ++(*iter);

    /* form an array of values-arguments ​​for print statement */
    args = pli_alloc(MAX_PRINT_ARGS * sizeof(value_t));
    if(!args)
    {
        add_err_code(MEMORY_ALLOCATION_ERROR, tokens[*iter].line_number, false);
        return MEMORY_ALLOCATION_ERROR;
    }

    /* collecting arguments as tokens */
    collect_args(tokens, &iter);

    /* collect all arguments */
    while (args_count < MAX_PRINT_ARGS)
    {
        print_add_arg(tokens[*iter], args, &args_count);

        /* if token comma ... */
    }

    return SUCCESS;
}