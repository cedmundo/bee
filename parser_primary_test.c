//
// Created by carlos on 9/9/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"

static void test_parse_id(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_id", "an_id");
    rest = bee_token_next(start, &error);
    node = bee_parse_id_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(node->as_str, "an_id");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_id", "123");
    rest = bee_token_next(start, &error);
    node = bee_parse_id_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U32_EXPR);
    assert_int_equal(node->as_u32, (uint32_t)123);
    bee_ast_node_free(node);
}

static void test_parse_path(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_path", "a.b.c");
    rest = bee_token_next(start, &error);
    node = bee_parse_path_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_PATH_EXPR);
    assert_int_equal(bee_dynamic_array_count(node->as_array), 3);
    struct bee_ast_node *element = NULL;
    assert_int_equal(bee_dynamic_array_get(node->as_array, 0, &element), BEE_DA_STATUS_OK);
    assert_int_equal(element->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(element->as_str, "a");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 1, &element), BEE_DA_STATUS_OK);
    assert_int_equal(element->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(element->as_str, "b");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 2, &element), BEE_DA_STATUS_OK);
    assert_int_equal(element->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(element->as_str, "c");
    bee_ast_node_free(node);
}

static void test_parse_primary(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_path", "(a.b.c)");
    rest = bee_token_next(start, &error);
    node = bee_parse_primary_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_PATH_EXPR);
    assert_int_equal(bee_dynamic_array_count(node->as_array), 3);
    struct bee_ast_node *element = NULL;
    assert_int_equal(bee_dynamic_array_get(node->as_array, 0, &element), BEE_DA_STATUS_OK);
    assert_int_equal(element->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(element->as_str, "a");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 1, &element), BEE_DA_STATUS_OK);
    assert_int_equal(element->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(element->as_str, "b");

    assert_int_equal(bee_dynamic_array_get(node->as_array, 2, &element), BEE_DA_STATUS_OK);
    assert_int_equal(element->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(element->as_str, "c");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_id", "(name)");
    rest = bee_token_next(start, &error);
    node = bee_parse_primary_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(node->as_str, "name");
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_id", "(123)");
    rest = bee_token_next(start, &error);
    node = bee_parse_primary_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U32_EXPR);
    assert_int_equal(node->as_u32, (uint32_t)123);
    bee_ast_node_free(node);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_id),
            cmocka_unit_test(test_parse_path),
            cmocka_unit_test(test_parse_primary),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}