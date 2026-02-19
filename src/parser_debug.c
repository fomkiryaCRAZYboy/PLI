#include "parser.h"

#include <stdio.h>

#define INDENT_STEP 2

static void print_indent(int depth)
{
    for(int i = 0; i < depth * INDENT_STEP; ++i)
        putchar(' ');
}

static const char* binary_op_str(binary_op_t op)
{
    switch(op)
    {
        case OP_AND:           return "and";
        case OP_OR:            return "or";
        case OP_EQUAL:         return "==";
        case OP_NOT_EQUAL:     return "!=";
        case OP_LESS:          return "<";
        case OP_GREATER:       return ">";
        case OP_LESS_EQUAL:    return "<=";
        case OP_GREATER_EQUAL: return ">=";
        case OP_ADD:           return "+";
        case OP_SUBTRACT:      return "-";
        case OP_MULTIPLY:      return "*";
        case OP_DIVIDE:        return "/";
        case OP_MODULO:        return "%%";
        default:               return "?";
    }
}

static const char* unary_op_str(unary_op_t op)
{
    switch(op)
    {
        case OP_NEGATE: return "-";
        case OP_NOT:    return "not";
        default:        return "?";
    }
}

static void print_expr(expr_node_t* expr, int depth);
static void print_stmt(stmt_node_t* stmt, int depth);

static void print_expr(expr_node_t* expr, int depth)
{
    if(!expr)
    {
        print_indent(depth);
        printf("<null>\n");
        return;
    }

    switch(expr->type)
    {
        case EXPR_LITERAL:
        {
            literal_expr_t* lit = expr->expr.literal;
            print_indent(depth);
            switch(lit->type)
            {
                case LIT_NUMBER:
                    printf("%.10g\n", lit->value.number);
                    break;
                case LIT_STRING:
                    printf("%s\n", lit->value.string);
                    break;
                case LIT_BOOLEAN:
                    printf("%s\n", lit->value.boolean ? "true" : "false");
                    break;
            }
            break;
        }

        case EXPR_VARIABLE:
            print_indent(depth);
            printf("%s\n", expr->expr.variable->name);
            break;

        case EXPR_BINARY:
            print_indent(depth);
            printf("[%s]\n", binary_op_str(expr->expr.binary->op));
            print_expr(expr->expr.binary->left,  depth + 1);
            print_expr(expr->expr.binary->right, depth + 1);
            break;

        case EXPR_UNARY:
            print_indent(depth);
            printf("[%s]\n", unary_op_str(expr->expr.unary->op));
            print_expr(expr->expr.unary->operand, depth + 1);
            break;

        case EXPR_GROUPING:
            print_indent(depth);
            printf("[group]\n");
            print_expr(expr->expr.grouping->expression, depth + 1);
            break;
    }
}

static void print_stmt(stmt_node_t* stmt, int depth)
{
    if(!stmt) return;

    switch(stmt->type)
    {
        case STMT_VAR_DECL:
            print_indent(depth);
            printf("var %s =\n", stmt->as.var_decl->var_name);
            print_expr(stmt->as.var_decl->initializer, depth + 1);
            break;

        case STMT_ASSIGNMENT:
            print_indent(depth);
            printf("%s =\n", stmt->as.assignment->var_name);
            print_expr(stmt->as.assignment->value, depth + 1);
            break;

        case STMT_IF:
            print_indent(depth);
            printf("if\n");
            print_expr(stmt->as.if_stmt->condition, depth + 1);
            print_indent(depth);
            printf("then\n");
            print_stmt(stmt->as.if_stmt->then_branch, depth + 1);
            if(stmt->as.if_stmt->else_branch)
            {
                print_indent(depth);
                printf("else\n");
                print_stmt(stmt->as.if_stmt->else_branch, depth + 1);
            }
            break;

        case STMT_WHILE:
            print_indent(depth);
            printf("while\n");
            print_expr(stmt->as.while_stmt->condition, depth + 1);
            print_indent(depth);
            printf("body\n");
            print_stmt(stmt->as.while_stmt->body, depth + 1);
            break;

        case STMT_PRINT:
            print_indent(depth);
            printf("print (%d args)\n", stmt->as.print_stmt->expr_count);
            for(int i = 0; i < stmt->as.print_stmt->expr_count; ++i)
                print_expr(stmt->as.print_stmt->expressions[i], depth + 1);
            break;

        case STMT_READ:
            print_indent(depth);
            printf("read(%s)\n", stmt->as.read_stmt->var_name);
            break;

        case STMT_BLOCK:
            print_indent(depth);
            printf("{\n");
            for(int i = 0; i < stmt->as.block->stmt_count; ++i)
                print_stmt(stmt->as.block->statements[i], depth + 1);
            print_indent(depth);
            printf("}\n");
            break;
    }
}

void print_ast(program_t* program)
{
    if(!program)
    {
        printf("=== AST: <null> ===\n");
        return;
    }

    printf("=== AST (%d statements) ===\n", program->stmt_count);

    stmt_node_t* curr = program->statements;
    while(curr)
    {
        print_stmt(curr, 1);
        curr = curr->next;
    }

    printf("========================\n");
}
