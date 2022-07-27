//
// Created by carlos on 7/26/22.
//
#include <stddef.h>
#include <jit/jit-util.h>

#include "type_check.h"

struct bee_type_expr *bee_type_expr_new() {
    return jit_calloc(1, sizeof(struct bee_type_expr));
}

static struct bee_type_expr *bee_type_expr_new_scalar(const char *id) {
    struct bee_type_expr *expr = bee_type_expr_new();
    expr->type = BEE_TYPE_EXPR_SCALAR;
    expr->id = id;
    return expr;
}

static struct bee_type_expr *bee_type_expr_new_error(const char *filename, size_t row, size_t col, const char *fmt, ...) {
    va_list(args);
    va_start(args, fmt);
    struct bee_error error;
    bee_set_error_va(&error, BEE_ERROR_TYPE_CHECK, filename, row, col, fmt, args);
    va_end(args);

    struct bee_type_expr *expr = bee_type_expr_new();
    expr->type = BEE_TYPE_EXPR_ERROR;
    expr->error = error;
    return expr;
}

// static void bee_type_expr_free(struct bee_type_expr *expr) {
//    jit_free(expr);
// }

static bool bee_type_expr_equal(struct bee_type_expr *a, struct bee_type_expr *b) {
    if (a->type != b->type) {
        return false;
    }

    switch(a->type) {
        case BEE_TYPE_EXPR_SCALAR:
            return jit_strcmp(a->id, b->id) == 0;
        case BEE_TYPE_EXPR_PRODUCT:
        case BEE_TYPE_EXPR_ARROW:
        case BEE_TYPE_EXPR_DECL:
        case BEE_TYPE_EXPR_NONE:
        case BEE_TYPE_EXPR_ERROR:
        case BEE_TYPE_EXPR_TYPES:
            return false;
    }
}

struct bee_type_expr *bee_type_check(struct bee_ast_node *node) {
    struct bee_type_expr *type_expr = NULL;
    switch (node->type) {
        case BEE_AST_NODE_ID:
            type_expr = bee_type_expr_new_scalar(node->as_str);
            break;
        case BEE_AST_NODE_LIT_BOL:
            type_expr =  bee_type_expr_new_scalar("Boolean");
            break;
        case BEE_AST_NODE_LIT_U8:
            type_expr = bee_type_expr_new_scalar("U8");
            break;
        case BEE_AST_NODE_LIT_U16:
            type_expr = bee_type_expr_new_scalar("U16");
            break;
        case BEE_AST_NODE_LIT_U32:
            type_expr = bee_type_expr_new_scalar("U32");
            break;
        case BEE_AST_NODE_LIT_U64:
            type_expr = bee_type_expr_new_scalar("U64");
            break;
        case BEE_AST_NODE_LIT_I8:
            type_expr = bee_type_expr_new_scalar("I8");
            break;
        case BEE_AST_NODE_LIT_I16:
            type_expr = bee_type_expr_new_scalar("I16");
            break;
        case BEE_AST_NODE_LIT_I32:
            type_expr = bee_type_expr_new_scalar("I32");
            break;
        case BEE_AST_NODE_LIT_I64:
            type_expr = bee_type_expr_new_scalar("I64");
            break;
        case BEE_AST_NODE_LIT_F32:
            type_expr = bee_type_expr_new_scalar("F32");
            break;
        case BEE_AST_NODE_LIT_F64:
            type_expr = bee_type_expr_new_scalar("F64");
            break;
        case BEE_AST_NODE_LIT_STR:
            type_expr = bee_type_expr_new_scalar("String");
            break;
        case BEE_AST_NODE_UNA_ARI_POS:
        case BEE_AST_NODE_UNA_ARI_NEG:
        case BEE_AST_NODE_UNA_LOG_NEG:
        case BEE_AST_NODE_UNA_BIT_NEG:
            type_expr = bee_type_check(node->left);
            break;
        case BEE_AST_NODE_BIN_ADD:
        case BEE_AST_NODE_BIN_SUB:
        case BEE_AST_NODE_BIN_MUL:
        case BEE_AST_NODE_BIN_DIV:
        case BEE_AST_NODE_BIN_REM:
        case BEE_AST_NODE_BIN_NE:
        case BEE_AST_NODE_BIN_EQ:
        case BEE_AST_NODE_BIN_LT:
        case BEE_AST_NODE_BIN_LE:
        case BEE_AST_NODE_BIN_GT:
        case BEE_AST_NODE_BIN_GE:
        case BEE_AST_NODE_BIN_RSH:
        case BEE_AST_NODE_BIN_LSH:
        case BEE_AST_NODE_BIN_LOG_OR:
        case BEE_AST_NODE_BIN_LOG_AND:
        case BEE_AST_NODE_BIN_BIT_AND:
        case BEE_AST_NODE_BIN_BIT_OR:
        case BEE_AST_NODE_BIN_BIT_XOR:
            type_expr = bee_type_check(node->left);
            if (!bee_type_expr_equal(type_expr, bee_type_check(node->right))) {
                // TODO: type as string
                // TODO: node type as string
                type_expr = bee_type_expr_new_error(node->filename,node->row,node->col,
                                                    "cannot perform `[binary]` operation on types `[left]` and `[right]`");
            }
            break;
        case BEE_AST_NODE_BIN_DUCK_ASSIGN:
        case BEE_AST_NODE_LET_IN_EXPR:
        case BEE_AST_NODE_CALL:
        case BEE_AST_NODE_ARG:
        case BEE_AST_NODE_TYPES:
        case BEE_AST_NODE_NONE:
            type_expr = bee_type_expr_new_error(node->filename,node->row,node->col,
                                                "unsupported operation to type check`");
            break;
    }

    return type_expr;
}