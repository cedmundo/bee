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

static void test_parse_call_noargs(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_noargs", "a()");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_CALL_EXPR);
    assert_int_equal(bee_dynamic_array_count(node->as_array), 1);
    assert_int_equal(bee_dynamic_array_get(node->as_array, 0, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_noargs", "(a)()");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_CALL_EXPR);
    assert_int_equal(bee_dynamic_array_count(node->as_array), 1);
    assert_int_equal(bee_dynamic_array_get(node->as_array, 0, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_noargs", "a.b.c()");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_CALL_EXPR);
    assert_int_equal(bee_dynamic_array_count(node->as_array), 1);
    assert_int_equal(bee_dynamic_array_get(node->as_array, 0, &aux0), BEE_DA_STATUS_OK);

    assert_int_equal(aux0->tag, BEE_AST_NODE_PATH_EXPR);
    assert_int_equal(bee_dynamic_array_count(aux0->as_array), 3);

    assert_int_equal(bee_dynamic_array_get(aux0->as_array, 0, &aux1), BEE_DA_STATUS_OK);
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "a");

    assert_int_equal(bee_dynamic_array_get(aux0->as_array, 1, &aux1), BEE_DA_STATUS_OK);
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");

    assert_int_equal(bee_dynamic_array_get(aux0->as_array, 2, &aux1), BEE_DA_STATUS_OK);
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_call_single_arg(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0, *aux1;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_noargs", "a(b)");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_CALL_EXPR);
    assert_int_equal(bee_dynamic_array_count(node->as_array), 2);

    assert_int_equal(bee_dynamic_array_get(node->as_array, 0, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 1, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_noargs", "a(123)");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_CALL_EXPR);
    assert_int_equal(bee_dynamic_array_count(node->as_array), 2);

    assert_int_equal(bee_dynamic_array_get(node->as_array, 0, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 1, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_LIT_U32_EXPR);
    assert_int_equal(aux0->as_u32, (uint32_t)123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_noargs", "a(b())");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_CALL_EXPR);
    assert_int_equal(bee_dynamic_array_count(node->as_array), 2);

    assert_int_equal(bee_dynamic_array_get(node->as_array, 0, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 1, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_CALL_EXPR);
    assert_int_equal(bee_dynamic_array_count(aux0->as_array), 1);

    // fetch callee
    assert_int_equal(bee_dynamic_array_get(aux0->as_array, 0, &aux1), BEE_DA_STATUS_OK);
    assert_int_equal(aux1->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux1->as_str, "b");
    bee_ast_node_free(node);
}

static void test_parse_call_multiple_args(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_noargs", "a(b, c, d)");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_CALL_EXPR);
    assert_int_equal(bee_dynamic_array_count(node->as_array), 4);

    assert_int_equal(bee_dynamic_array_get(node->as_array, 0, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 1, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "b");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 2, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "c");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 3, &aux0), BEE_DA_STATUS_OK);
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "d");
    bee_ast_node_free(node);
}

static void test_parse_call_unclosing_arg_error(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_noargs", "a(b");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_expr(&rest, &error);
    assert_null(node);
    assert_true(bee_error_is_set(&error));
    assert_string_equal(error.msg, "unclosed parenthesis, opening pair was at 1:2");

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_noargs", "a(b,");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_expr(&rest, &error);
    assert_null(node);
    assert_true(bee_error_is_set(&error));
    assert_string_equal(error.msg, "unclosed parenthesis, opening pair was at 1:2");
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_call_noargs),
            cmocka_unit_test(test_parse_call_single_arg),
            cmocka_unit_test(test_parse_call_multiple_args),
            cmocka_unit_test(test_parse_call_unclosing_arg_error),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}