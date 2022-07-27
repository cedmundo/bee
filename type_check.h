//
// Created by carlos on 7/26/22.
//

#ifndef BEE_TYPE_CHECK_H
#define BEE_TYPE_CHECK_H

#include "parser.h"

enum bee_type_expr_type {
    BEE_TYPE_EXPR_NONE,
    BEE_TYPE_EXPR_ERROR,
    BEE_TYPE_EXPR_SCALAR,
    BEE_TYPE_EXPR_PRODUCT,
    BEE_TYPE_EXPR_ARROW,
    BEE_TYPE_EXPR_DECL,
    BEE_TYPE_EXPR_TYPES,
};

struct bee_type_expr {
    union {
        const char *id;
        struct {
            struct bee_type_expr *left;
            struct bee_type_expr *right;
        };
        struct bee_error error;
    };
    enum bee_type_expr_type type;
};

struct bee_type_expr *bee_type_expr_new();
struct bee_type_expr *bee_type_check(struct bee_ast_node *node);

#endif //BEE_TYPE_CHECK_H
