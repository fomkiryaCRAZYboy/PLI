#include "errs.h"
#include "lexer.h"
#include "mem.h"
#include "parser_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int read_from_file(const char* path, char* program)
{
    FILE* f = fopen(path, "r");
    if(!f)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", path);
        return -1;
    }

    size_t total_read = 0;
    char buffer[MAX_TOKENS_COUNT_IN_BLOCK];

    while(fgets(buffer, sizeof(buffer), f) != NULL && total_read < MAX_TOKENS_COUNT_IN_BLOCK - 1)
    {
        size_t line_length = strlen(buffer);
        size_t available = MAX_TOKENS_COUNT_IN_BLOCK - total_read - 1;

        if(line_length > available)
            line_length = available;

        memcpy(program + total_read, buffer, line_length);
        total_read += line_length;

        if(total_read >= MAX_TOKENS_COUNT_IN_BLOCK - 1)
            break;
    }

    fclose(f);
    program[total_read] = '\0';
    return 0;
}

static void read_interactive(char* program)
{
    printf("Enter your PLI program (empty line to finish):\n1 > ");

    size_t total_read = 0;
    int lnum = 2;
    char buffer[1024];

    while(fgets(buffer, sizeof(buffer), stdin) != NULL && total_read < MAX_TOKENS_COUNT_IN_BLOCK - 1)
    {
        int is_empty = 1;
        for(char* p = buffer; *p; p++)
        {
            if(*p != '\n' && *p != ' ' && *p != '\t' && *p != '\r')
            {
                is_empty = 0;
                break;
            }
        }

        if(is_empty && total_read > 0)
            break;

        size_t line_length = strlen(buffer);
        size_t available = MAX_TOKENS_COUNT_IN_BLOCK - total_read - 1;

        if(line_length > available)
            line_length = available;

        memcpy(program + total_read, buffer, line_length);
        total_read += line_length;

        if(total_read >= MAX_TOKENS_COUNT_IN_BLOCK - 1)
            break;

        printf("%d > ", lnum++);
    }

    program[total_read] = '\0';
}

int main(int argc, char* argv[])
{
    int ret = atexit_registration();
    if(ret != 0) exit(EXIT_FAILURE);

    char program[MAX_TOKENS_COUNT_IN_BLOCK];
    memset(program, 0, sizeof(program));

    if(argc > 1)
    {
        if(read_from_file(argv[1], program) != 0)
            exit(EXIT_FAILURE);
    }
    else
    {
        read_interactive(program);
    }

    TOKEN_STREAM* stream = tokenize(program);
    if(!stream)
    {
        add_err_code(MAIN_func_TOKENIZE_ERROR, 0, false);
        exit(EXIT_FAILURE);
    }

    program_t* ast = parse(stream);

    print_ast(ast);

    if(stream)
    {
        if(stream->tokens)
            pli_free(stream->tokens);
        pli_free(stream);
    }

    exit(EXIT_SUCCESS);
}
