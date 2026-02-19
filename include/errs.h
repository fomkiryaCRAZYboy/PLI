#ifndef ERR_H
#define ERR_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_FAILS   20

#define WARNINGS \
        DECODE_w(SUCCESS_w,                             "Success") \
        DECODE_w(PLI_FREE_func_UNTRACKED_POINTER,       "TryingToFreeUntrackedPointer")
    

#define ERRORS \
        DECODE(SUCCESS,                                      "Success") \
        DECODE(MEMORY_ALLOCATION_ERROR,                      "MemoryAllocationError") \
        \
        DECODE(MAIN_func_TOKENIZE_ERROR,                     "TokenizeError") \
        \
        DECODE(CREATE_TOKEN_func_STREAM_OVERFLOW_ERROR,      "TokenStreamOverflowError") \
        \
        DECODE(TOKENIZE_func_STREAM_CREATION_ERROR,          "StreamCreationError") \
        DECODE(TOKENIZE_func_GET_NUMBER_TOKEN_ERROR,         "GetNumberTokenError") \
        DECODE(TOKENIZE_func_GET_TEXT_TOKEN_ERROR,           "GetTextTokenError") \
        DECODE(TOKENIZE_func_GET_SEP_OP_TOKEN_ERROR,         "GetSepOpTokenError") \
        DECODE(TOKENIZE_func_GET_STR_TOKEN_ERROR,            "GetStringTokenError") \
        DECODE(TOKENIZE_func_NUM_TOKEN_CREATION_ERROR,       "NumberTokenCreationError") \
        DECODE(TOKENIZE_func_TEXT_TOKEN_CREATION_ERROR,      "TextTokenCreationError") \
        DECODE(TOKENIZE_func_SEP_OP_TOKEN_CREATION_ERROR,    "SepOpTokenCreationError") \
        DECODE(TOKENIZE_func_STRING_TOKEN_CREATION_ERROR,    "StringTokenCreationError") \
        \
        DECODE(GET_NUMBER_TOKEN_func_ALLOC_ERROR,            "NumberTokenAllocError") \
        DECODE(GET_NUMBER_TOKEN_func_BIG_NUMBER_ERROR,       "BigNumberError") \
        DECODE(GET_NUMBER_TOKEN_func_INVALID_NUMBER,         "InvalidNumberError") \
        \
        DECODE(GET_TEXT_TOKEN_func_ALLOC_ERROR,              "IdentifierTokenAllocError") \
        DECODE(GET_TEXT_TOKEN_func_INVALID_IDENTIFIER_ERROR, "InvalidIdentifierError") \
        DECODE(GET_TEXT_TOKEN_func_LONG_IDENTIFIER_ERROR,    "TooLongIdentifierError") \
        DECODE(GET_TEXT_TOKEN_func_LONG_STRING_ERROR,        "TooLongStringError") \
        DECODE(GET_TEXT_TOKEN_func_QUOTE_NOT_FOUND_ERROR,    "ClosingQouteNotFoundError") \
        \
        DECODE(GET_SEP_OP_TOKEN_func_ALLOC_ERROR,            "SepOpTokenAllocError") \
        DECODE(GET_SEP_OP_TOKEN_func_INVALID_SEP_OP_ERROR,   "InvalidSepOpError") \
        \
        DECODE(PARSE_func_CRAETE_VAR_DECL_STMT_ERROR,        "VarDelarationStatementCreationError") \
        DECODE(PARSE_func_CRAETE_ASSIGNMENT_STMT_ERROR,      "AssignmentStatementCreationError") \
        DECODE(PARSE_func_STMT_CREATION_ERROR,               "StatementCreationError") \
        \
        DECODE(CREATE_VAR_DECL_STMT_func_FIRST_TOKEN_NOT_VAR_ERROR,      "MissingVarKeyWordError") \
        DECODE(CREATE_VAR_DECL_STMT_func_SECOND_TOKEN_NOT_IDEN_ERROR,    "MissingIdentifierError") \
        DECODE(CREATE_VAR_DECL_STMT_func_THIRD_TOKEN_NOT_ASSIGN_ERROR,   "MissingAssignmentSignError") \
        DECODE(CREATE_VAR_DECL_STMT_func_INITIALIZER_PARSE_ERROR,        "InitializerParseError") \
        \
        DECODE(CREATE_ASSIGNMENT_STMT_func_FIRST_TOKEN_NOT_IDEN_ERROR,       "MissingIdentifierError") \
        DECODE(CREATE_ASSIGNMENT_STMT_func_SECOND_TOKEN_NOT_ASSIGN_ERROR,    "MissingAssignmentSignError") \
        DECODE(CREATE_ASSIGNMENT_STMT_func_ASSIGNMENT_VALUE_PARSE_ERROR,     "AssignmentValueParseError") \
        \
        DECODE(CREATE_IF_STMT_func_FIRST_TOKEN_NOT_IF_ERROR,             "MissingIfKeywordError")\
        DECODE(CREATE_IF_STMT_func_SECOND_TOKEN_NOT_LPAREN_ERROR,        "MissingOpeningParenthesisError") \
        \
        DECODE(CREATE_EXPR_NODE_func_INVALID_EXPRESSION,                 "InvalidExpressionError") \
        \
        DECODE(PARSE_EXPR_UNEXPECTED_TOKEN,                              "UnexpectedTokenInExpression") \
        DECODE(PARSE_EXPR_MISSING_RPAREN,                                "MissingClosingParenthesis") \
        DECODE(PARSE_EXPR_MISSING_OPERAND,                               "MissingOperandAfterOperator") \
        \
        DECODE(CREATE_STMT_func_UNKNOWN_STMT_TYPE,                       "UnknownStatementType") \
        \
        DECODE(PARSING_func_MISSING_LPAREN_PRINT_ST_ERROR,   "MissingOpeningParenthesisError") \
        DECODE(PARSING_func_INVALID_ARG_PRINT_ST_ERROR,      "InvalidPrintArgumentError") \
        \
        DECODE(PRINT_ADD_ARG_func_VARIABLE_NOT_EXIST_ERROR,  "VariableDoesNotExist") \


/* generate enum for all kinds of warnings */        
typedef enum
{
    #ifdef  DECODE_w
    #undef  DECODE_w
    #endif

    #define DECODE_w(code, description) code,
    WARNINGS
    #undef  DECODE_w
    WARNINGS_CODES_COUNT
} 
warnings ;

/* generate enum for all kinds of errors */
typedef enum
{
    #ifdef  DECODE
    #undef  DECODE
    #endif

    #define DECODE(code, description) code,
    ERRORS
    #undef  DECODE
    ERRORS_CODES_COUNT
} 
f_result ;

/*  the structure stores the error 
    code and the function that 
    terminated with this error */
typedef struct 
{
    short  err_code;
    short  line_num;
    bool   warning;
}
err_code ;

void add_err_code(int error_code, int line_num, bool warning);
void print_errors(void);
char* decode_err(int error_code, bool warning);
int atexit_registration(void);

#endif /* ERR_H */