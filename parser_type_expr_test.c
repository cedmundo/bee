//
// Created by carlos on 9/12/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"

static void test_parse_primary(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_primary", "A");
    rest = bee_token_next(start, &error);
    node = bee_parse_primary_type_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(node->as_str, "A");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_primary", "(A)");
    rest = bee_token_next(start, &error);
    node = bee_parse_primary_type_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(node->as_str, "A");
    bee_ast_node_free(node);
}

static void test_parse_binary_mul_type_expr(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_mul_type_expr", "A, B");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_type_expr(&rest, &error, 0);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_MUL_TYPE_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "A");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "B");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_mul_type_expr", "A, B, C");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_type_expr(&rest, &error, 0);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_MUL_TYPE_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_MUL_TYPE_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "A");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "B");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "C");
    bee_ast_node_free(node);
}

static void test_parse_binary_add_type_expr(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_add_type_expr", "A | B");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_type_expr(&rest, &error, 1);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_ADD_TYPE_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "A");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "B");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_add_type_expr", "A | B | C");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_type_expr(&rest, &error, 1);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_ADD_TYPE_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_ADD_TYPE_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "A");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "B");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "C");
    bee_ast_node_free(node);
}

static void test_parse_binary_decl_type_expr(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_decl_type_expr", "A : B");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_type_expr(&rest, &error, 2);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_DECL_TYPE_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "A");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "B");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_decl_type_expr", "A : B : C");
    rest = bee_token_next(start, &error);
    node = bee_parse_binary_type_expr(&rest, &error, 2);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_DECL_TYPE_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_DECL_TYPE_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "A");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "B");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "C");
    bee_ast_node_free(node);
}

static void test_parse_type_expr(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_binary_decl_type_expr", "a: A, b: (B | C)");
    rest = bee_token_next(start, &error);
    node = bee_parse_type_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_BIN_MUL_TYPE_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_DECL_TYPE_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "a");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "A");

    aux0 = node->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_BIN_DECL_TYPE_EXPR);

    aux1 = aux0->as_pair.left;
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    aux1 = aux0->as_pair.right;
    assert_int_equal(aux1->tag, BEE_AST_NODE_BIN_ADD_TYPE_EXPR);

    aux0 = aux1->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "B");

    aux0 = aux1->as_pair.right;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "C");
    bee_ast_node_free(node);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_primary),
            cmocka_unit_test(test_parse_binary_mul_type_expr),
            cmocka_unit_test(test_parse_binary_add_type_expr),
            cmocka_unit_test(test_parse_binary_decl_type_expr),
            cmocka_unit_test(test_parse_type_expr),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
