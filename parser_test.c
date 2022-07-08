//
// Created by carlos on 6/25/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "parser.h"

static void test_parse_primary(void **state) {
    (void) state;

    struct bee_token start = bee_token_start("test", "id 123 \"a string\" (2.2) (-1)");
    struct bee_token token = bee_token_next(start);
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node;

    node = bee_parse_primary(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_ID);
    assert_string_equal("id", node->as_str);
    bee_ast_node_free(node);

    node = bee_parse_primary(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_LIT_U32);
    assert_int_equal(node->as_u32, 123);
    bee_ast_node_free(node);

    node = bee_parse_primary(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_LIT_STR);
    assert_string_equal("\"a string\"", node->as_str);
    bee_ast_node_free(node);

    node = bee_parse_primary(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_LIT_F32);
    assert_float_equal(2.2, node->as_f32, 0.001);
    bee_ast_node_free(node);

    node = bee_parse_primary(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_LIT_I32);
    assert_int_equal(node->as_i32, -1);
    bee_ast_node_free(node);
}

static void test_parse_unary(void **state) {
    (void) state;

    struct bee_token start = bee_token_start("test", "+id -id ~id +-id");
    struct bee_token token = bee_token_next(start);
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux;

    node = bee_parse_unary(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_UNA_ARI_POS);

    aux = node->left;
    assert_non_null(aux);
    assert_int_equal(aux->type, BEE_AST_NODE_ID);
    assert_string_equal("id", aux->as_str);
    bee_ast_node_free(node);

    node = bee_parse_unary(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_UNA_ARI_NEG);

    aux = node->left;
    assert_non_null(aux);
    assert_int_equal(aux->type, BEE_AST_NODE_ID);
    assert_string_equal("id", aux->as_str);
    bee_ast_node_free(node);

    node = bee_parse_unary(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_UNA_BIT_NEG);

    aux = node->left;
    assert_non_null(aux);
    assert_int_equal(aux->type, BEE_AST_NODE_ID);
    assert_string_equal("id", aux->as_str);
    bee_ast_node_free(node);

    node = bee_parse_unary(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_UNA_ARI_POS);

    aux = node->left;
    assert_non_null(aux);
    assert_int_equal(aux->type, BEE_AST_NODE_UNA_ARI_NEG);

    aux = aux->left;
    assert_non_null(aux);
    assert_int_equal(aux->type, BEE_AST_NODE_ID);
    assert_string_equal("id", aux->as_str);
    bee_ast_node_free(node);
}

static void test_parse_mul(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "a * b");
    token = bee_token_next(start);
    node = bee_parse_mul(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_MUL);

    aux0 = node->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);

    aux1 = node->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    bee_ast_node_free(node);

    start = bee_token_start("test", "a * b / c % d");
    token = bee_token_next(start);
    node = bee_parse_mul(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_REM);

    aux0 = node->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("d", aux0->as_str);

    aux1 = node->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_DIV);

    aux0 = aux1->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux0->as_str);

    aux1 = aux1->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_MUL);

    aux0 = aux1->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux0->as_str);

    aux1 = aux1->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux1->as_str);
    bee_ast_node_free(node);

    start = bee_token_start("test", "a * +b");
    token = bee_token_next(start);
    node = bee_parse_mul(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_MUL);

    aux0 = node->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux0->as_str);

    aux1 = node->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_UNA_ARI_POS);

    aux0 = aux1->left;
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux0->as_str);
    bee_ast_node_free(node);

    start = bee_token_start("test", "a * (b / c)");
    token = bee_token_next(start);
    node = bee_parse_mul(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_MUL);

    aux0 = node->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux0->as_str);

    aux1 = node->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_DIV);

    aux0 = aux1->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux0->as_str);

    aux1 = aux1->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);
    bee_ast_node_free(node);
}

static void test_parse_add(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "a + b");
    token = bee_token_next(start);
    node = bee_parse_add(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_ADD);

    aux0 = node->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux0->as_str);

    aux1 = node->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);
    bee_ast_node_free(node);

    start = bee_token_start("test", "a + b * c");
    token = bee_token_next(start);
    node = bee_parse_add(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_ADD);

    aux0 = node->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux0->as_str);

    aux1 = node->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_MUL);

    aux0 = aux1->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux0->as_str);

    aux1 = aux1->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux1->as_str);
    bee_ast_node_free(node);

    start = bee_token_start("test", "a * b - c * d + f");
    token = bee_token_next(start);
    node = bee_parse_add(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_ADD);

    aux0 = node->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("f", aux0->as_str);

    aux1 = node->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_SUB);

    aux0 = aux1->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_MUL);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);

    aux0 = node->left->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_MUL);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("d", aux1->as_str);
    bee_ast_node_free(node);
}

static void test_parse_shift(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "a + b << c - d >> f");
    token = bee_token_next(start);
    node = bee_parse_shift(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_RSH);

    aux0 = node->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("f", aux0->as_str);

    aux1 = node->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_LSH);

    aux0 = aux1->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_ADD);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);

    aux0 = node->left->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_SUB);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("d", aux1->as_str);
    bee_ast_node_free(node);
}

static void test_parse_bit_and(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "a << b & c >> d & f");
    token = bee_token_next(start);
    node = bee_parse_bit_and(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_BIT_AND);

    aux0 = node->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("f", aux0->as_str);

    aux1 = node->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_BIT_AND);

    aux0 = aux1->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_LSH);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);

    aux0 = node->left->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_RSH);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("d", aux1->as_str);
    bee_ast_node_free(node);
}

static void test_parse_bit_xor(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "a & b ^ c & d ^ f");
    token = bee_token_next(start);
    node = bee_parse_bit_xor(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_BIT_XOR);

    aux0 = node->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("f", aux0->as_str);

    aux1 = node->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_BIT_XOR);

    aux0 = aux1->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_BIT_AND);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);

    aux0 = node->left->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_BIT_AND);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("d", aux1->as_str);
    bee_ast_node_free(node);
}

static void test_parse_bit_or(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "a ^ b | c ^ d | f");
    token = bee_token_next(start);
    node = bee_parse_bit_or(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_BIT_OR);

    aux0 = node->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("f", aux0->as_str);

    aux1 = node->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_BIT_OR);

    aux0 = aux1->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_BIT_XOR);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);

    aux0 = node->left->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_BIT_XOR);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("d", aux1->as_str);
    bee_ast_node_free(node);
}

static void test_parse_rel(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "a | b == c | d != f");
    token = bee_token_next(start);
    node = bee_parse_rel(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_NE);

    aux0 = node->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("f", aux0->as_str);

    aux1 = node->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_EQ);

    aux0 = aux1->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_BIT_OR);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);

    aux0 = node->left->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_BIT_OR);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("d", aux1->as_str);
    bee_ast_node_free(node);
}

static void test_parse_log_not(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "not a == b");
    token = bee_token_next(start);
    node = bee_parse_log_not(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_UNA_LOG_NEG);

    aux0 = node->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_EQ);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);
    bee_ast_node_free(node);
}

static void test_parse_log_and(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "a and not b and c");
    token = bee_token_next(start);
    node = bee_parse_log_and(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_LOG_AND);

    aux0 = node->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux0->as_str);

    aux1 = node->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_LOG_AND);

    aux0 = aux1->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux0->as_str);

    aux0 = aux1->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_UNA_LOG_NEG);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);
    bee_ast_node_free(node);
}

static void test_parse_log_or(void **state) {
    (void) state;

    struct bee_token start;
    struct bee_token token;
    struct bee_parser_error error = {0LL};
    struct bee_ast_node *node, *aux0, *aux1;

    start = bee_token_start("test", "a and b or c and d or f");
    token = bee_token_next(start);
    node = bee_parse_log_or(&token, &error);
    assert_int_equal(error.type, BEE_PARSER_ERROR_NONE);
    assert_non_null(node);
    assert_int_equal(node->type, BEE_AST_NODE_BIN_LOG_OR);

    aux0 = node->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_ID);
    assert_string_equal("f", aux0->as_str);

    aux1 = node->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_BIN_LOG_OR);

    aux0 = aux1->left;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_LOG_AND);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("a", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("b", aux1->as_str);

    aux0 = node->left->right;
    assert_non_null(aux0);
    assert_int_equal(aux0->type, BEE_AST_NODE_BIN_LOG_AND);

    aux1 = aux0->left;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("c", aux1->as_str);

    aux1 = aux0->right;
    assert_non_null(aux1);
    assert_int_equal(aux1->type, BEE_AST_NODE_ID);
    assert_string_equal("d", aux1->as_str);
    bee_ast_node_free(node);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_primary),
            cmocka_unit_test(test_parse_unary),
            cmocka_unit_test(test_parse_mul),
            cmocka_unit_test(test_parse_add),
            cmocka_unit_test(test_parse_shift),
            cmocka_unit_test(test_parse_bit_and),
            cmocka_unit_test(test_parse_bit_xor),
            cmocka_unit_test(test_parse_bit_or),
            cmocka_unit_test(test_parse_rel),
            cmocka_unit_test(test_parse_log_not),
            cmocka_unit_test(test_parse_log_and),
            cmocka_unit_test(test_parse_log_or),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}