//
// Created by carlos on 9/10/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"

static void test_parse_max_prec(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_max_prec", "+a");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 8);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_UNA_ARI_POS_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");
    bee_ast_node_free(node);
}

static void test_parse_binary_mul(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_mul", "a * b");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 8);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_MUL_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_mul", "a * b * c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 8);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_MUL_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_MUL_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "a");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "c");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_mul", "a * b / c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 8);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_DIV_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_MUL_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "a");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_binary_add(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_add", "a + b");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 7);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_ADD_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_add", "a + b * c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 7);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_ADD_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_MUL_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_add", "a + (b * c)");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 7);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_ADD_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_MUL_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_binary_bit_shift(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_bit_shift", "a << b");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 6);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_LSH_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_bit_shift", "a >> b + c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 6);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_RSH_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_ADD_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_bit_shift", "a >> (b + c)");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 6);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_RSH_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_ADD_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_binary_bit_and(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_bit_and", "a & b");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 5);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_BIT_AND_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_bit_and", "a & b << c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 5);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_BIT_AND_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_LSH_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_binary_bit_xor(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_bit_xor", "a ^ b");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 4);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_BIT_XOR_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_bit_xor", "a ^ b & c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 4);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_BIT_XOR_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_BIT_AND_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_binary_bit_or(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_bit_or", "a | b");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 3);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_BIT_OR_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_bit_or", "a | b ^ c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 3);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_BIT_OR_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_BIT_XOR_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_binary_eq(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_eq", "a == b");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 2);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_EQ_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_eq", "a == b | c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 2);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_EQ_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_BIT_OR_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_binary_log_and(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_log_and", "a && b");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 1);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_LOG_AND_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_log_and", "a && b == c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 1);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_LOG_AND_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_EQ_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_binary_log_or(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_log_or", "a || b");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 0);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_LOG_OR_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_log_or", "a || b && c");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_expr(&rest, &error, 0);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_LOG_OR_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_LOG_AND_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_expr(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_expr", "a == (b0 - b1) && (c0 * c1) == -d");
    rest = bee_token_next(start, &error);
    node = bee_parse_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    bee_ast_node_free(node);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_max_prec),
            cmocka_unit_test(test_parse_binary_mul),
            cmocka_unit_test(test_parse_binary_add),
            cmocka_unit_test(test_parse_binary_bit_shift),
            cmocka_unit_test(test_parse_binary_bit_and),
            cmocka_unit_test(test_parse_binary_bit_xor),
            cmocka_unit_test(test_parse_binary_bit_or),
            cmocka_unit_test(test_parse_binary_eq),
            cmocka_unit_test(test_parse_binary_log_or),
            cmocka_unit_test(test_parse_binary_log_and),
            cmocka_unit_test(test_parse_expr),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
