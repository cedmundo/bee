//
// Created by carlos on 6/25/22.
//

#ifndef BEE_PARSER_H
#define BEE_PARSER_H
#include <stdint.h>
#include <stdbool.h>
#include "lexer.h"
#include "errors.h"

enum bee_ast_node_type {
    BEE_AST_NODE_NONE,
    BEE_AST_NODE_ID,
    BEE_AST_NODE_LIT_BOL,
    BEE_AST_NODE_LIT_I8,
    BEE_AST_NODE_LIT_I16,
    BEE_AST_NODE_LIT_I32,
    BEE_AST_NODE_LIT_I64,
    BEE_AST_NODE_LIT_U8,
    BEE_AST_NODE_LIT_U16,
    BEE_AST_NODE_LIT_U32,
    BEE_AST_NODE_LIT_U64,
    BEE_AST_NODE_LIT_F32,
    BEE_AST_NODE_LIT_F64,
    BEE_AST_NODE_LIT_STR,
    BEE_AST_NODE_UNA_LOG_NEG,
    BEE_AST_NODE_UNA_BIT_NEG,
    BEE_AST_NODE_UNA_ARI_NEG,
    BEE_AST_NODE_UNA_ARI_POS,
    BEE_AST_NODE_BIN_ADD,
    BEE_AST_NODE_BIN_SUB,
    BEE_AST_NODE_BIN_MUL,
    BEE_AST_NODE_BIN_DIV,
    BEE_AST_NODE_BIN_REM,
    BEE_AST_NODE_BIN_EQ,
    BEE_AST_NODE_BIN_NE,
    BEE_AST_NODE_BIN_GT,
    BEE_AST_NODE_BIN_GE,
    BEE_AST_NODE_BIN_LT,
    BEE_AST_NODE_BIN_LE,
    BEE_AST_NODE_BIN_LSH,
    BEE_AST_NODE_BIN_RSH,
    BEE_AST_NODE_BIN_BIT_AND,
    BEE_AST_NODE_BIN_BIT_OR,
    BEE_AST_NODE_BIN_BIT_XOR,
    BEE_AST_NODE_BIN_LOG_AND,
    BEE_AST_NODE_BIN_LOG_OR,
    BEE_AST_NODE_BIN_DUCK_ASSIGN,
    BEE_AST_NODE_LET_IN_EXPR,
    BEE_AST_NODE_CALL,
    BEE_AST_NODE_ARG,
    BEE_AST_NODE_TYPES
};

struct bee_ast_node {
    const char *filename;
    union {
        struct {
            struct bee_ast_node *left;
            struct bee_ast_node *right;
        };
        char *as_str;
        uint8_t as_u8;
        uint16_t as_u16;
        uint32_t as_u32;
        uint64_t as_u64;
        int8_t as_i8;
        int16_t as_i16;
        int32_t as_i32;
        int64_t as_i64;
        float as_f32;
        double as_f64;
        bool as_bol;
    };
    size_t row;
    size_t col;
    enum bee_ast_node_type type;
};

struct bee_ast_node *bee_ast_node_new();
struct bee_ast_node *bee_ast_node_new_unary(struct bee_token at_token, enum bee_ast_node_type type, struct bee_ast_node *left);
struct bee_ast_node *bee_ast_node_new_binary(struct bee_token at_token, enum bee_ast_node_type type, struct bee_ast_node *left, struct bee_ast_node *right);

void bee_ast_node_free(struct bee_ast_node *node);

struct bee_ast_node *bee_parse_expr(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_let_in(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_log_or(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_log_and(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_log_not(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_rel(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_bit_or(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_bit_xor(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_bit_and(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_shift(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_add(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_mul(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_unary(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_call_or_lit(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_call(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_args(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_primary(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_must_parse_id(struct bee_token *rest, struct bee_error *error);
#endif //BEE_PARSER_H
