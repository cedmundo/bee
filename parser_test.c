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

}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_primary),
            cmocka_unit_test(test_parse_unary),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}