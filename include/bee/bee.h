#ifndef BEE_H
#define BEE_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <jit/jit-value.h>

enum bee_ast_node_type {
    BEE_AST_NODE_TYPE_I32,
    BEE_AST_NODE_TYPE_ADD,
    BEE_AST_NODE_TYPE_SUB,
    BEE_AST_NODE_TYPE_MUL,
    BEE_AST_NODE_TYPE_DIV,
    BEE_AST_NODE_TYPE_MOD,
    BEE_AST_NODE_TYPE_BIT_AND,
    BEE_AST_NODE_TYPE_BIT_OR,
    BEE_AST_NODE_TYPE_BIT_XOR,
    BEE_AST_NODE_TYPE_BIT_LSHIFT,
    BEE_AST_NODE_TYPE_BIT_RSHIFT,
    BEE_AST_NODE_TYPE_EQ,
    BEE_AST_NODE_TYPE_NEQ,
    BEE_AST_NODE_TYPE_LT,
    BEE_AST_NODE_TYPE_LTE,
    BEE_AST_NODE_TYPE_GT,
    BEE_AST_NODE_TYPE_GTE,
    BEE_AST_NODE_TYPE_LOG_AND,
    BEE_AST_NODE_TYPE_LOG_OR,
    BEE_AST_NODE_TYPE_ARI_INV_POS,
    BEE_AST_NODE_TYPE_ARI_INV_NEG,
    BEE_AST_NODE_TYPE_LOG_NEG,
    BEE_AST_NODE_TYPE_BIT_NEG,
};

struct bee_ast_node {
    struct bee_ast_node *left;
    struct bee_ast_node *right;
    int32_t as_i32;
    enum bee_ast_node_type type;
};

struct bee_ast_node *bee_ast_node_new();
void bee_ast_node_print(struct bee_ast_node *node, size_t depth);
void bee_ast_node_free(struct bee_ast_node *node);

struct bee_ast_node *bee_parse_expr(const char **rest);
struct bee_ast_node *bee_parse_logical_expr(const char **rest);
struct bee_ast_node *bee_parse_comparison_expr(const char **rest);
struct bee_ast_node *bee_parse_inequality_expr(const char **rest);
struct bee_ast_node *bee_parse_bitwise_expr(const char **rest);
struct bee_ast_node *bee_parse_bitshift_expr(const char **rest);
struct bee_ast_node *bee_parse_factor_expr(const char **rest);
struct bee_ast_node *bee_parse_term_expr(const char **rest);
struct bee_ast_node *bee_parse_unary_expr(const char **rest);
struct bee_ast_node *bee_parse_primary_expr(const char **rest);
struct bee_ast_node *bee_parse_number_lit(const char **rest);
size_t bee_parse_spaces(const char **rest);
bool bee_parse_digit(const char **rest);
bool bee_parse_space(const char **rest);

jit_value_t bee_compile_expr(jit_function_t f, struct bee_ast_node *node);
#endif /* BEE_H */