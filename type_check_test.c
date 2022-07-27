//
// Created by carlos on 7/26/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "parser.h"
#include "type_check.h"

static void test_type_check_literals(void **state) {
    (void) state;

    struct bee_token start = bee_token_start("test", "1 1.2 true \"hello\" 0xFF_u8");
    struct bee_token token = bee_token_next(start);
    struct bee_error error = {.type = BEE_ERROR_NONE};
    struct bee_ast_node *node;
    struct bee_type_expr *type_expr;

    node = bee_parse_expr(&token, &error);
    assert_int_equal(error.type, BEE_ERROR_NONE);
    assert_non_null(node);
    type_expr = bee_type_check(node);
    assert_non_null(type_expr);
    assert_int_equal(type_expr->type, BEE_TYPE_EXPR_SCALAR);
    assert_string_equal(type_expr->id, "U32");
    bee_ast_node_free(node);

    node = bee_parse_expr(&token, &error);
    assert_int_equal(error.type, BEE_ERROR_NONE);
    assert_non_null(node);
    type_expr = bee_type_check(node);
    assert_non_null(type_expr);
    assert_int_equal(type_expr->type, BEE_TYPE_EXPR_SCALAR);
    assert_string_equal(type_expr->id, "F32");
    bee_ast_node_free(node);

    node = bee_parse_expr(&token, &error);
    assert_int_equal(error.type, BEE_ERROR_NONE);
    assert_non_null(node);
    type_expr = bee_type_check(node);
    assert_non_null(type_expr);
    assert_int_equal(type_expr->type, BEE_TYPE_EXPR_SCALAR);
    assert_string_equal(type_expr->id, "Boolean");
    bee_ast_node_free(node);

    node = bee_parse_expr(&token, &error);
    assert_int_equal(error.type, BEE_ERROR_NONE);
    assert_non_null(node);
    type_expr = bee_type_check(node);
    assert_non_null(type_expr);
    assert_int_equal(type_expr->type, BEE_TYPE_EXPR_SCALAR);
    assert_string_equal(type_expr->id, "String");
    bee_ast_node_free(node);

    node = bee_parse_expr(&token, &error);
    assert_int_equal(error.type, BEE_ERROR_NONE);
    assert_non_null(node);
    type_expr = bee_type_check(node);
    assert_non_null(type_expr);
    assert_int_equal(type_expr->type, BEE_TYPE_EXPR_SCALAR);
    assert_string_equal(type_expr->id, "U8");
    bee_ast_node_free(node);
}

static void test_type_check_unary_operators(void **state) {
    (void) state;

    struct bee_token start = bee_token_start("test", "-(0.1) not true");
    struct bee_token token = bee_token_next(start);
    struct bee_error error = {.type = BEE_ERROR_NONE};
    struct bee_ast_node *node;
    struct bee_type_expr *type_expr;

    node = bee_parse_expr(&token, &error);
    assert_int_equal(error.type, BEE_ERROR_NONE);
    assert_non_null(node);
    type_expr = bee_type_check(node);
    assert_non_null(type_expr);
    assert_int_equal(type_expr->type, BEE_TYPE_EXPR_SCALAR);
    assert_string_equal(type_expr->id, "F32");
    bee_ast_node_free(node);

    node = bee_parse_expr(&token, &error);
    assert_int_equal(error.type, BEE_ERROR_NONE);
    assert_non_null(node);
    type_expr = bee_type_check(node);
    assert_non_null(type_expr);
    assert_int_equal(type_expr->type, BEE_TYPE_EXPR_SCALAR);
    assert_string_equal(type_expr->id, "Boolean");
    bee_ast_node_free(node);
}

static void test_type_check_binary_operators(void **state) {
    (void) state;

    struct bee_token start = bee_token_start("test", "1 + 1");
    struct bee_token token = bee_token_next(start);
    struct bee_error error = {.type = BEE_ERROR_NONE};
    struct bee_ast_node *node;
    struct bee_type_expr *type_expr;

    node = bee_parse_expr(&token, &error);
    assert_int_equal(error.type, BEE_ERROR_NONE);
    assert_non_null(node);
    type_expr = bee_type_check(node);
    assert_non_null(type_expr);
    assert_int_equal(type_expr->type, BEE_TYPE_EXPR_SCALAR);
    assert_string_equal(type_expr->id, "U32");
    bee_ast_node_free(node);

    start = bee_token_start("test", "1.0 + \"should type error\"");
    token = bee_token_next(start);
    error.type = BEE_ERROR_NONE;

    node = bee_parse_expr(&token, &error);
    assert_int_equal(error.type, BEE_ERROR_NONE);
    assert_non_null(node);
    type_expr = bee_type_check(node);
    assert_non_null(type_expr);
    assert_int_equal(type_expr->type, BEE_TYPE_EXPR_ERROR);
    bee_ast_node_free(node);
}

int main() {
    // NOTE: for unused imports
    (void) (jmp_buf *) 0;
    (void) (va_list *) 0;

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_type_check_literals),
            cmocka_unit_test(test_type_check_unary_operators),
            cmocka_unit_test(test_type_check_binary_operators),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
