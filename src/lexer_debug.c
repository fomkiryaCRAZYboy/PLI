#include "lexer.h"
#include <stdio.h>

const char* get_type_of_token(TOKEN_TYPE type) {
    switch (type) {
        /* --- KEYWORDS --- */
        case kw_token_var:        return "KW_VAR";
        case kw_token_if:         return "KW_IF";
        case kw_token_else:       return "KW_ELSE";
        case kw_token_while:      return "KW_WHILE";
        case kw_token_print:      return "KW_PRINT";
        case kw_token_read:       return "KW_READ";
        case kw_token_and:        return "KW_AND";
        case kw_token_or:         return "KW_OR";
        case kw_token_not:        return "KW_NOT";
        case kw_token_end:        return "KW_END";

        /* --- IDENTIFIERS --- */
        case iden_token:          return "ID";

        /* --- LITERALS --- */
        case str_token:           return "STR";
        case num_token:           return "NUM";
        case bool_token_t:        return "BOOL_T";
        case bool_token_f:        return "BOOL_F";
        
        /* --- ARITHMETIC OPERATORS --- */
        case math_op_token_mult:  return "OP_MULT";
        case math_op_token_div:   return "OP_DIV";
        case math_op_token_plus:  return "OP_PLUS";
        case math_op_token_minus: return "OP_MINUS";

        /* --- ASSIGNMENT OPERATOR --- */
        case assing_op_token:     return "OP_ASSIGN";

        /* --- COMPARISON OPERATORS --- */
        case comp_op_token_equal:      return "OP_EQ";
        case comp_op_token_not_equal:  return "OP_NE";
        case comp_op_token_more:       return "OP_GT";
        case comp_op_token_less:       return "OP_LT";
        case comp_op_token_less_equal: return "OP_LE";
        case comp_op_token_more_equal: return "OP_GE";

        /* --- SEPARATORS --- */
        case sep_token_lparen:    return "SEP_LPAREN";
        case sep_token_rparen:    return "SEP_RPAREN";
        case sep_token_lbrace:    return "SEP_LBRACE";
        case sep_token_rbrace:    return "SEP_RBRACE";
        case sep_token_comma:     return "SEP_COMMA";
        case sep_token_semicolon: return "SEP_SEMICOL";
        case sep_token_dot:       return "SEP_DOT";

        /* --- SPECIAL --- */
        case unknown_token:       return "UNKNOWN";

        default:                  return "UNKNOWN_TYPE";
    }
}

void debug_print_stream(TOKEN* stream, int* tokens_count)
{
    printf("---------------------\n");
    printf("tokens_count = %d\n", *tokens_count);
    printf("---------------------\n");
    for(int i = 0; i < *tokens_count; i++){
        if (stream[i].type_token == unknown_token || stream[i].type_token < 12)
            printf("[%s](%s), ", get_type_of_token(stream[i].type_token), stream[i].text_token.text);
        else if (stream[i].type_token >= math_op_token_mult)
            printf("[%s](%s), ", get_type_of_token(stream[i].type_token), stream[i].text_token.sep_and_op_text);        
        else if (stream[i].type_token == bool_token_f || stream[i].type_token == bool_token_t)
            printf("[%s](%s), ", get_type_of_token(stream[i].type_token), stream[i].text_token.bool_text);     
        else
            printf("[%s](%s), ", get_type_of_token(stream[i].type_token), stream[i].text_token.num_text);     
    }
}