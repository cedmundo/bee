//
// Created by carlos on 9/26/21.
//
#include <bee.h>
#include <jit/jit-common.h>
#include <jit/jit-insn.h>
#include <jit/jit-type.h>
#include <jit/jit-util.h>
#include <jit/jit-value.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

struct bee_ast_node *bee_ast_node_new() {
    return (struct bee_ast_node *) jit_calloc(sizeof(struct bee_ast_node), 1);
}

struct bee_ast_node *ast_node_new_binary(enum bee_ast_node_type type, struct bee_ast_node *left, struct bee_ast_node *right) {
    struct bee_ast_node *node = bee_ast_node_new();
    node->type = type;
    node->left = left;
    node->right = right;
    return node;
}

struct bee_ast_node *ast_node_new_unary(enum bee_ast_node_type type, struct bee_ast_node *left) {
    struct bee_ast_node *node = bee_ast_node_new();
    node->type = type;
    node->left = left;
    return node;
}

void bee_ast_node_free(struct bee_ast_node *node) {
    assert(node != NULL);
    if (node->left != NULL) {
        bee_ast_node_free(node->left);
    }

    if (node->right != NULL) {
        bee_ast_node_free(node->right);
    }

    free(node);
}

void bee_ast_node_print(struct bee_ast_node *node, size_t depth) {
    for(size_t i=0;i<depth;i++) printf(" ");

    if (node == NULL) {
        return;
    }

    static const char *node_type_strings[] = {
            "I32",
            "ADD",
            "SUB",
            "MUL",
            "DIV",
            "MOD",
            "BIT AND",
            "BIT OR",
            "BIT XOR",
            "BIT LSHIFT",
            "BIT RSHIFT",
            "EQ",
            "NEQ",
            "LT",
            "LTE",
            "GT",
            "GTE",
            "LOGICAL AND",
            "LOGICAL OR",
    };

    if (node->type == BEE_AST_NODE_TYPE_I32) {
        printf("[I32 %d]\n", node->as_i32);
    } else {
        printf("[%s]\n", node_type_strings[node->type]);
    }

    if (node->left != NULL) {
        bee_ast_node_print(node->left, depth + 1);
    }

    if (node->right != NULL) {
        bee_ast_node_print(node->right, depth + 1);
    }
}

bool match(const char **rest, const char *seq) {
    size_t seq_len = jit_strlen(seq);
    if (jit_memcmp(seq, *rest, seq_len) == 0) {
        *rest += seq_len;
        return true;
    }

    return false;
}

struct bee_ast_node *bee_parse_expr(const char **rest) {
    return bee_parse_logical_expr(rest);
}

struct bee_ast_node *bee_parse_logical_expr(const char **rest) {
    struct bee_ast_node *node = bee_parse_comparison_expr(rest);
    for(;;) {
        bee_parse_spaces(rest);

        if (match(rest, "||")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_LOG_OR,
                    node,
                    bee_parse_comparison_expr(rest)
            );
            continue;
        }

        if (match(rest, "&&")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_LOG_AND,
                    node,
                    bee_parse_comparison_expr(rest)
            );
            continue;
        }

        return node;
    }
}

struct bee_ast_node *bee_parse_comparison_expr(const char **rest) {
    struct bee_ast_node *node = bee_parse_inequality_expr(rest);
    for(;;) {
        bee_parse_spaces(rest);

        if (match(rest, "==")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_EQ,
                    node,
                    bee_parse_inequality_expr(rest)
            );
            continue;
        }

        if (match(rest, "!=")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_NEQ,
                    node,
                    bee_parse_inequality_expr(rest)
            );
            continue;
        }

        return node;
    }
}

struct bee_ast_node *bee_parse_inequality_expr(const char **rest) {
    struct bee_ast_node *node = bee_parse_bitwise_expr(rest);
    for(;;) {
        bee_parse_spaces(rest);

        if (match(rest, ">=")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_GTE,
                    node,
                    bee_parse_bitwise_expr(rest)
            );
            continue;
        }

        if (match(rest, "<=")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_LTE,
                    node,
                    bee_parse_bitwise_expr(rest)
            );
            continue;
        }

        if (match(rest, ">")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_GT,
                    node,
                    bee_parse_bitwise_expr(rest)
            );
            continue;
        }

        if (match(rest, "<")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_LT,
                    node,
                    bee_parse_bitwise_expr(rest)
            );
            continue;
        }

        return node;
    }
}

struct bee_ast_node *bee_parse_bitwise_expr(const char **rest) {
    struct bee_ast_node *node = bee_parse_bitshift_expr(rest);
    for(;;) {
        bee_parse_spaces(rest);

        if (match(rest, "& ")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_BIT_AND,
                    node,
                    bee_parse_bitshift_expr(rest)
            );
            continue;
        }

        if (match(rest, "| ")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_BIT_OR,
                    node,
                    bee_parse_bitshift_expr(rest)
            );
            continue;
        }

        if (match(rest, "^")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_BIT_XOR,
                    node,
                    bee_parse_bitshift_expr(rest)
            );
            continue;
        }


        return node;
    }
}

struct bee_ast_node *bee_parse_bitshift_expr(const char **rest) {
    struct bee_ast_node *node = bee_parse_factor_expr(rest);
    for(;;) {
        bee_parse_spaces(rest);

        if (match(rest, "<<")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_BIT_LSHIFT,
                    node,
                    bee_parse_factor_expr(rest)
            );
            continue;
        }

        if (match(rest, ">>")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_BIT_RSHIFT,
                    node,
                    bee_parse_factor_expr(rest)
            );
            continue;
        }

        return node;
    }
}

struct bee_ast_node *bee_parse_factor_expr(const char **rest) {
    struct bee_ast_node *node = bee_parse_term_expr(rest);
    for(;;) {
        bee_parse_spaces(rest);

        if (match(rest, "-")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_SUB,
                    node,
                    bee_parse_term_expr(rest)
            );
            continue;
        }

        if (match(rest, "+")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_ADD,
                    node,
                    bee_parse_term_expr(rest)
            );
            continue;
        }

        return node;
    }
}

struct bee_ast_node *bee_parse_term_expr(const char **rest) {
    struct bee_ast_node *node = bee_parse_unary_expr(rest);
    for(;;) {
        bee_parse_spaces(rest);

        if (match(rest, "*")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_MUL,
                    node,
                    bee_parse_unary_expr(rest)
            );
            continue;
        }

        if (match(rest, "/")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_DIV,
                    node,
                    bee_parse_unary_expr(rest)
            );
            continue;
        }

        if (match(rest, "%")) {
            node = ast_node_new_binary(
                    BEE_AST_NODE_TYPE_MOD,
                    node,
                    bee_parse_unary_expr(rest)
            );
            continue;
        }

        return node;
    }
}

struct bee_ast_node *bee_parse_unary_expr(const char **rest) {
    bee_parse_spaces(rest);

    if (match(rest, "!")) {
        return ast_node_new_unary(BEE_AST_NODE_TYPE_LOG_NEG, bee_parse_unary_expr(rest));
    }

    if (match(rest, "~")) {
        return ast_node_new_unary(BEE_AST_NODE_TYPE_BIT_NEG, bee_parse_unary_expr(rest));
    }

    if (match(rest, "-")) {
        return ast_node_new_unary(BEE_AST_NODE_TYPE_ARI_INV_NEG, bee_parse_unary_expr(rest));
    }

    if (match(rest, "+")) {
        return ast_node_new_unary(BEE_AST_NODE_TYPE_ARI_INV_POS, bee_parse_unary_expr(rest));
    }

    return bee_parse_primary_expr(rest);
}

struct bee_ast_node *bee_parse_primary_expr(const char **rest) {
    bee_parse_spaces(rest);

    if (match(rest, "(")) {
        struct bee_ast_node *node = bee_parse_expr(rest);
        if (!match(rest, ")")) {
            printf("error: expecting an ')', found: '%c'\n", **rest);
        }
        return node;
    }

    return bee_parse_number_lit(rest);
}

struct bee_ast_node *bee_parse_number_lit(const char **rest) {
    const char *base_digits = *rest;
    for (; bee_parse_digit(rest););

    // FIXME: use count_digits and a own version of atoi
    if (base_digits == *rest) {
        return NULL;
    }

    struct bee_ast_node *node = bee_ast_node_new();
    node->type = BEE_AST_NODE_TYPE_I32;
    node->as_i32 = 0;
    node->as_i32 = (int32_t)strtol(base_digits, NULL, 10);
    return node;
}

bool bee_parse_digit(const char **rest) {
    return (
            match(rest, "0") ||
            match(rest, "1") ||
            match(rest, "2") ||
            match(rest, "3") ||
            match(rest, "4") ||
            match(rest, "5") ||
            match(rest, "6") ||
            match(rest, "7") ||
            match(rest, "8") ||
            match(rest, "9")
    );
}

bool bee_parse_space(const char **rest) {
    return match(rest, " ") || match(rest, "\t");
}

size_t bee_parse_spaces(const char **rest) {
    size_t count = 0;
    for(; bee_parse_space(rest); count++);
    return count;
}

jit_value_t bee_compile_expr(jit_function_t f, struct bee_ast_node *node) {
    jit_value_t value;
    switch (node->type) {
        case BEE_AST_NODE_TYPE_I32:
            value = jit_value_create_nint_constant(f, jit_type_int, node->as_i32);
            break;
        case BEE_AST_NODE_TYPE_ADD:
            value = jit_insn_add(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_SUB:
            value = jit_insn_sub(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_MUL:
            value = jit_insn_mul(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_DIV:
            value = jit_insn_div(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_MOD:
            value = jit_insn_rem(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_AND:
            value = jit_insn_and(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_OR:
            value = jit_insn_or(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_XOR:
            value = jit_insn_xor(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_LSHIFT:
            value = jit_insn_shl(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_RSHIFT:
            value = jit_insn_shr(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_EQ:
            value = jit_insn_eq(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_NEQ:
            value = jit_insn_ne(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_GT:
            value = jit_insn_gt(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_GTE:
            value = jit_insn_ge(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_LT:
            value = jit_insn_lt(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_LTE:
            value = jit_insn_le(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_LOG_AND:
        {
            jit_value_t tmp = jit_value_create(f, jit_type_sys_bool);
            jit_label_t a_label = jit_label_undefined;
            jit_label_t b_label = jit_label_undefined;
            jit_insn_branch_if(f, bee_compile_expr(f, node->left), &a_label);
            jit_insn_store(f, tmp, jit_value_create_nint_constant(f, jit_type_sys_bool, 0));
            jit_insn_branch(f, &b_label);

            jit_insn_label(f, &a_label);
            jit_insn_store(f, tmp, bee_compile_expr(f, node->right));

            jit_insn_label(f, &b_label);
            value = jit_insn_load(f, tmp);
            break;
        }
        case BEE_AST_NODE_TYPE_LOG_OR:
        {
            jit_value_t tmp = jit_value_create(f, jit_type_sys_bool);
            jit_label_t a_label = jit_label_undefined;
            jit_label_t b_label = jit_label_undefined;
            jit_insn_branch_if_not(f, bee_compile_expr(f, node->left), &a_label);
            jit_insn_store(f, tmp, jit_value_create_nint_constant(f, jit_type_sys_bool, 1));
            jit_insn_branch(f, &b_label);

            jit_insn_label(f, &a_label);
            jit_insn_store(f, tmp, bee_compile_expr(f, node->right));

            jit_insn_label(f, &b_label);
            value = jit_insn_load(f, tmp);
            break;
        }
        case BEE_AST_NODE_TYPE_ARI_INV_POS:
            value = jit_insn_mul(f, bee_compile_expr(f, node->left), jit_value_create_nint_constant(f, jit_type_int, +1));
            break;
        case BEE_AST_NODE_TYPE_ARI_INV_NEG:
            value = jit_insn_mul(f, bee_compile_expr(f, node->left), jit_value_create_nint_constant(f, jit_type_int, -1));
            break;
        case BEE_AST_NODE_TYPE_BIT_NEG:
            value = jit_insn_not(f, bee_compile_expr(f, node->left));
            break;
        case BEE_AST_NODE_TYPE_LOG_NEG:
            value = jit_insn_neg(f, bee_compile_expr(f, node->left));
            break;
    }
    return value;
}
