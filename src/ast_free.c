#include "parser.h"
#include "mem.h"

#include <stddef.h>

static void free_stmt_list(stmt_node_t* head);
static void free_stmt_node(stmt_node_t* s);

static void free_expr(expr_node_t* e)
{
    if (!e)
        return;

    switch (e->type)
    {
        case EXPR_BINARY:
            if (e->expr.binary)
            {
                free_expr(e->expr.binary->left);
                free_expr(e->expr.binary->right);
                pli_free(e->expr.binary);
            }
            break;
        case EXPR_UNARY:
            if (e->expr.unary)
            {
                free_expr(e->expr.unary->operand);
                pli_free(e->expr.unary);
            }
            break;
        case EXPR_LITERAL:
            if (e->expr.literal)
                pli_free(e->expr.literal);
            break;
        case EXPR_VARIABLE:
            if (e->expr.variable)
                pli_free(e->expr.variable);
            break;
        case EXPR_GROUPING:
            if (e->expr.grouping)
            {
                free_expr(e->expr.grouping->expression);
                pli_free(e->expr.grouping);
            }
            break;
        default:
            break;
    }
    pli_free(e);
}

/* Free one statement node and all owned AST (does not follow ->next). */
static void free_stmt_node(stmt_node_t* s)
{
    if (!s)
        return;

    switch (s->type)
    {
        case STMT_VAR_DECL:
            if (s->as.var_decl)
            {
                free_expr(s->as.var_decl->initializer);
                pli_free(s->as.var_decl);
            }
            break;
        case STMT_ASSIGNMENT:
            if (s->as.assignment)
            {
                free_expr(s->as.assignment->value);
                pli_free(s->as.assignment);
            }
            break;
        case STMT_IF:
            if (s->as.if_stmt)
            {
                free_expr(s->as.if_stmt->condition);
                free_stmt_node(s->as.if_stmt->then_branch);
                free_stmt_node(s->as.if_stmt->else_branch);
                pli_free(s->as.if_stmt);
            }
            break;
        case STMT_WHILE:
            if (s->as.while_stmt)
            {
                free_expr(s->as.while_stmt->condition);
                free_stmt_node(s->as.while_stmt->body);
                pli_free(s->as.while_stmt);
            }
            break;
        case STMT_PRINT:
            if (s->as.print_stmt)
            {
                print_stmt_t* ps = s->as.print_stmt;
                if (ps->expressions)
                {
                    for (int i = 0; i < ps->expr_count; i++)
                        free_expr(ps->expressions[i]);
                    pli_free(ps->expressions);
                }
                pli_free(ps);
            }
            break;
        case STMT_READ:
            if (s->as.read_stmt)
                pli_free(s->as.read_stmt);
            break;
        case STMT_BLOCK:
            if (s->as.block)
            {
                free_stmt_list(s->as.block->statements);
                pli_free(s->as.block);
            }
            break;
        default:
            break;
    }
    pli_free(s);
}

static void free_stmt_list(stmt_node_t* head)
{
    while (head)
    {
        stmt_node_t* next = head->next;
        free_stmt_node(head);
        head = next;
    }
}

void free_program(program_t* p)
{
    if (!p)
        return;
    free_stmt_list(p->statements);
    pli_free(p);
}
