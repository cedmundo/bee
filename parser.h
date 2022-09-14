//
// Created by carlos on 9/3/22.
//

#ifndef BEE_PARSER_H
#define BEE_PARSER_H
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "errors.h"
#include "array.h"
#include "lexer.h"

#define BEE_MAX_PARSING_DIGITS 48

enum bee_ast_node_tag {
    BEE_AST_NODE_NONE,
    BEE_AST_NODE_ID_EXPR,
    BEE_AST_NODE_PATH_EXPR,
    BEE_AST_NODE_CALL_EXPR,
    BEE_AST_NODE_LIT_BOL_EXPR,
    BEE_AST_NODE_LIT_I8_EXPR,
    BEE_AST_NODE_LIT_I16_EXPR,
    BEE_AST_NODE_LIT_I32_EXPR,
    BEE_AST_NODE_LIT_I64_EXPR,
    BEE_AST_NODE_LIT_U8_EXPR,
    BEE_AST_NODE_LIT_U16_EXPR,
    BEE_AST_NODE_LIT_U32_EXPR,
    BEE_AST_NODE_LIT_U64_EXPR,
    BEE_AST_NODE_LIT_F32_EXPR,
    BEE_AST_NODE_LIT_F64_EXPR,
    BEE_AST_NODE_LIT_STR_EXPR,
    BEE_AST_NODE_UNA_LOG_NEG_EXPR,
    BEE_AST_NODE_UNA_BIT_NEG_EXPR,
    BEE_AST_NODE_UNA_ARI_NEG_EXPR,
    BEE_AST_NODE_UNA_ARI_POS_EXPR,
    BEE_AST_NODE_BIN_ADD_EXPR,
    BEE_AST_NODE_BIN_SUB_EXPR,
    BEE_AST_NODE_BIN_MUL_EXPR,
    BEE_AST_NODE_BIN_DIV_EXPR,
    BEE_AST_NODE_BIN_REM_EXPR,
    BEE_AST_NODE_BIN_EQ_EXPR,
    BEE_AST_NODE_BIN_NE_EXPR,
    BEE_AST_NODE_BIN_GT_EXPR,
    BEE_AST_NODE_BIN_GE_EXPR,
    BEE_AST_NODE_BIN_LT_EXPR,
    BEE_AST_NODE_BIN_LE_EXPR,
    BEE_AST_NODE_BIN_LSH_EXPR,
    BEE_AST_NODE_BIN_RSH_EXPR,
    BEE_AST_NODE_BIN_BIT_AND_EXPR,
    BEE_AST_NODE_BIN_BIT_OR_EXPR,
    BEE_AST_NODE_BIN_BIT_XOR_EXPR,
    BEE_AST_NODE_BIN_LOG_AND_EXPR,
    BEE_AST_NODE_BIN_LOG_OR_EXPR,
    BEE_AST_NODE_BIN_ADD_TYPE_EXPR,
    BEE_AST_NODE_BIN_MUL_TYPE_EXPR,
    BEE_AST_NODE_BIN_DECL_TYPE_EXPR,
    BEE_AST_NODE_STATEMENTS_BLOCK,
    BEE_AST_NODE_IF_STMT,
    BEE_AST_NODE_COND_BLOCK_PAIR,
    BEE_AST_NODE_WHILE_STMT,
    BEE_AST_NODE_ASSERT_STMT,
    BEE_AST_NODE_RETURN_STMT,
    BEE_AST_NODE_CONTINUE_STMT,
    BEE_AST_NODE_BREAK_STMT,
    BEE_AST_NODE_ASSIGN_STMT,
    BEE_AST_NODE_INP_ADD_STMT,
    BEE_AST_NODE_INP_SUB_STMT,
    BEE_AST_NODE_INP_MUL_STMT,
    BEE_AST_NODE_INP_DIV_STMT,
    BEE_AST_NODE_INP_REM_STMT,
    BEE_AST_NODE_INP_AND_STMT,
    BEE_AST_NODE_INP_OR_STMT,
    BEE_AST_NODE_INP_XOR_STMT,
    BEE_AST_NODE_INP_LSH_STMT,
    BEE_AST_NODE_INP_RSH_STMT,
    BEE_AST_NODE_COUNT
};

struct bee_ast_node {
    struct bee_loc loc;
    union {
        struct {
          struct bee_ast_node *left;
          struct bee_ast_node *right;
        } as_pair;
        struct bee_array *as_array;
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
    enum bee_ast_node_tag tag;
};

struct bee_ast_node *bee_ast_node_new();
void bee_ast_node_free(void *node);
const char *bee_ast_node_tag_get_name(enum bee_ast_node_tag tag);

struct bee_ast_node *bee_parse_compilation_unit(struct bee_token start_token, struct bee_error *error);

struct bee_ast_node *bee_parse_stmts_block(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_stmt(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_if_stmt(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_while_stmt(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_ctrl_flow_stmt(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_call_or_assign_stmt(struct bee_token *rest, struct bee_error *error);

struct bee_ast_node *bee_parse_type_expr(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_binary_type_expr(struct bee_token *rest, struct bee_error *error, uint32_t prec);
struct bee_ast_node *bee_parse_primary_type_expr(struct bee_token *rest, struct bee_error *error);

struct bee_ast_node *bee_parse_expr(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_binary_expr(struct bee_token *rest, struct bee_error *error, uint32_t prec);
struct bee_ast_node *bee_parse_unary_expr(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_call_expr(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_primary_expr(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_path_expr(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_id_expr(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_parse_lit_expr(struct bee_token *rest, struct bee_error *error);
struct bee_ast_node *bee_must_parse_id_expr(struct bee_token *rest, struct bee_error *error);

#endif //BEE_PARSER_H
