//
// Created by carlos on 9/3/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"

static void test_parse_int_lit(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "123");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U32_EXPR);
    assert_int_equal(node->as_u32, (uint32_t)123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "123_u8");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U8_EXPR);
    assert_int_equal(node->as_u8, (uint8_t)123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "123_u16");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U16_EXPR);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->as_u16, (uint16_t)123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "123_u32");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U32_EXPR);
    assert_int_equal(node->as_u32, (uint32_t)123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "123_u64");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U64_EXPR);
    assert_int_equal(node->as_u64, (uint64_t)123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "-123");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_I32_EXPR);
    assert_int_equal(node->as_i32, (int32_t)-123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "-123_i8");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_I8_EXPR);
    assert_int_equal(node->as_i8, (int8_t)-123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "-123_i16");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_I16_EXPR);
    assert_int_equal(node->as_i16, (int16_t)-123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "-123_i32");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_I32_EXPR);
    assert_int_equal(node->as_i32, (int32_t)-123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "-123_i64");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_I64_EXPR);
    assert_int_equal(node->as_i64, (int64_t)-123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "123_i32");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_I32_EXPR);
    assert_int_equal(node->as_i32, (int32_t)123);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "0xFA");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U32_EXPR);
    assert_int_equal(node->as_u32, (uint32_t)0xFA);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "-0xCC");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_I32_EXPR);
    assert_int_equal(node->as_i32, (int32_t)-0xCC);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "0b10");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U32_EXPR);
    assert_int_equal(node->as_u32, (uint32_t)0b10);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "0o77");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U32_EXPR);
    assert_int_equal(node->as_u32, (uint32_t)077);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "-0o77_u16");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_U16_EXPR);
    assert_int_equal(node->as_u16, (uint16_t)-077);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_int_lit", "not_a_number");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_null(node);
    assert_false(bee_error_is_set(&error));
}


static void test_parse_flt_lit(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_flt_lit", "0.1");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_F32_EXPR);
    assert_float_equal(node->as_f32, 0.1, 0.00001);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_flt_lit", ".1");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_F32_EXPR);
    assert_float_equal(node->as_f32, 0.1, 0.00001);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_flt_lit", "-.1");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_F32_EXPR);
    assert_float_equal(node->as_f32, -0.1, 0.00001);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_flt_lit", "-0.");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_F32_EXPR);
    assert_float_equal(node->as_f32, -0.0, 0.00001);
    bee_ast_node_free(node);


    bee_error_clear(&error);
    start = bee_token_start("test_parse_flt_lit", "0.123_f64");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_F64_EXPR);
    assert_float_equal(node->as_f64, 0.123, 0.00001);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_flt_lit", "-.");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_null(node);
}

static void test_parse_bol_lit(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_bool_lit", "true");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_BOL_EXPR);
    assert_true(node->as_bol);
    bee_ast_node_free(node);

    bee_error_clear(&error);
    start = bee_token_start("test_parse_bool_lit", "false");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_BOL_EXPR);
    assert_false(node->as_bol);
    bee_ast_node_free(node);
}

static void test_parse_str_lit(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_str_lit", "\"an string\"");
    rest = bee_token_next(start, &error);
    node = bee_parse_lit_expr(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_LIT_STR_EXPR);
    assert_string_equal(node->as_str, "\"an string\"");
    bee_ast_node_free(node);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_int_lit),
            cmocka_unit_test(test_parse_bol_lit),
            cmocka_unit_test(test_parse_str_lit),
            cmocka_unit_test(test_parse_flt_lit),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}