#include "errs.h"
#include "lexer.h"
#include "mem.h"
#include "parser_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int ensure_capacity(char** program, size_t* capacity, size_t required_size)
{
    if(required_size <= *capacity)
        return 0;

    size_t new_capacity = *capacity;
    while(new_capacity < required_size)
        new_capacity *= 2;

    char* grown = pli_realloc(*program, new_capacity, *capacity);
    if(!grown)
        return -1;

    *program = grown;
    *capacity = new_capacity;
    return 0;
}

static int append_text(char** program, size_t* used, size_t* capacity, const char* text, size_t text_len)
{
    if(ensure_capacity(program, capacity, *used + text_len + 1) != 0)
        return -1;

    memcpy(*program + *used, text, text_len);
    *used += text_len;
    (*program)[*used] = '\0';
    return 0;
}

static char* read_from_file(const char* path)
{
    FILE* f = fopen(path, "r");
    if(!f)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", path);
        return NULL;
    }

    size_t capacity = 1024;
    size_t used = 0;
    char* program = pli_alloc(capacity);
    if(!program)
    {
        fclose(f);
        return NULL;
    }
    program[0] = '\0';

    char buffer[1024];

    while(fgets(buffer, sizeof(buffer), f) != NULL)
    {
        size_t line_length = strlen(buffer);
        if(append_text(&program, &used, &capacity, buffer, line_length) != 0)
        {
            fclose(f);
            pli_free(program);
            return NULL;
        }
    }

    fclose(f);
    return program;
}

static char* read_interactive(void)
{
    printf("Enter your PLI program (empty line to finish):\n1 > ");

    size_t capacity = 1024;
    size_t used = 0;
    char* program = pli_alloc(capacity);
    if(!program)
        return NULL;
    program[0] = '\0';

    int lnum = 2;
    char buffer[1024];

    while(fgets(buffer, sizeof(buffer), stdin) != NULL)
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

        if(is_empty && used > 0)
            break;

        size_t line_length = strlen(buffer);
        if(append_text(&program, &used, &capacity, buffer, line_length) != 0)
        {
            pli_free(program);
            return NULL;
        }

        printf("%d > ", lnum++);
    }

    return program;
}

int main(int argc, char* argv[])
{
    int ret = atexit_registration();
    if(ret != 0) exit(EXIT_FAILURE);

    char* program = NULL;

    if(argc > 1)
    {
        program = read_from_file(argv[1]);
        if(!program)
            exit(EXIT_FAILURE);
    }
    else
    {
        program = read_interactive();
        if(!program)
            exit(EXIT_FAILURE);
    }

    TOKEN_STREAM* stream = tokenize(program);
    if(!stream)
        exit(EXIT_FAILURE);

    program_t* ast = parse(stream);

    print_ast(ast);

    if(stream)
    {
        if(stream->tokens)
            pli_free(stream->tokens);
        pli_free(stream);
    }

    if(program)
        pli_free(program);

    exit(EXIT_SUCCESS);
}
