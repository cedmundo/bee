//
// Created by carlos on 7/6/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include <jit/jit.h>
#include "parser.h"
#include "compiler.h"

static bool eval_int_expr(const char *code, jit_int *result) {
    jit_context_t context = jit_context_create();

    jit_context_build_start(context);
    jit_type_t signature =
            jit_type_create_signature(jit_abi_cdecl, jit_type_int, NULL, 0, 1);
    jit_function_t function = jit_function_create(context, signature);
    jit_type_free(signature);

    struct bee_token start = bee_token_start("test", code);
    struct bee_token token = bee_token_next(start);
    struct bee_parser_error p_error = {.type = BEE_PARSER_ERROR_NONE};
    struct bee_ast_node *node = bee_parse_expr(&token, &p_error);
    if (p_error.type != BEE_PARSER_ERROR_NONE) {
        fprintf(stderr, "%s:%ld:%ld: parse error: %s", token.name, token.row, token.col, p_error.msg);
        return false;
    }

    struct bee_compiler_error c_error = {.type = BEE_COMPILER_ERROR_NONE};
    jit_value_t ret_value = bee_compile_node(function, node, &c_error);
    if (c_error.type != BEE_COMPILER_ERROR_NONE) {
        fprintf(stderr, "%s:%ld:%ld: compile error: %s", c_error.filename, c_error.row, c_error.col, c_error.msg);
        return false;
    }

    jit_insn_return(function, ret_value);

    jit_function_compile(function);
    jit_context_build_end(context);

    // Apply function
    jit_function_apply(function, NULL, result);
    return true;
}

static void test_compile_int_constants(void **state) {
    (void) state;

    jit_int result = 0;
    assert_true(eval_int_expr("123", &result));
    assert_int_equal(result, 123);

    assert_true(eval_int_expr("0x11", &result));
    assert_int_equal(result, 0x11);

    assert_true(eval_int_expr("-12", &result));
    assert_int_equal(result, -12);

    assert_true(eval_int_expr("true", &result));
    assert_int_equal(result, 1);
}

static void test_compile_binary_exprs(void **state) {
    (void) state;

    jit_int result = 0;
    assert_true(eval_int_expr("1 + 2", &result));
    assert_int_equal(result, 3);

    assert_true(eval_int_expr("5 - 3", &result));
    assert_int_equal(result, 2);

    assert_true(eval_int_expr("1 + -1", &result));
    assert_int_equal(result, 0);

    assert_true(eval_int_expr("true and true", &result));
    assert_int_equal(result, 1);

    assert_true(eval_int_expr("true and false", &result));
    assert_int_equal(result, 0);

    assert_true(eval_int_expr("false and true", &result));
    assert_int_equal(result, 0);

    assert_true(eval_int_expr("false and false", &result));
    assert_int_equal(result, 0);

    assert_true(eval_int_expr("true or true", &result));
    assert_int_equal(result, 1);

    assert_true(eval_int_expr("true or false", &result));
    assert_int_equal(result, 1);

    assert_true(eval_int_expr("false or true", &result));
    assert_int_equal(result, 1);

    assert_true(eval_int_expr("false or false", &result));
    assert_int_equal(result, 0);

    assert_true(eval_int_expr("0xBA & 0xAB", &result));
    assert_int_equal((uint8_t)result, 0xAA);

    assert_true(eval_int_expr("0xBA | 0xAB", &result));
    assert_int_equal((uint8_t)result, 0xBB);
}

static void test_compile_unary_exprs(void **state) {
    (void) state;

    jit_int result = 0;
    assert_true(eval_int_expr("not false", &result));
    assert_int_equal(result, 1);

    assert_true(eval_int_expr("not true", &result));
    assert_int_equal(result, 0);

    assert_true(eval_int_expr("-(1)", &result));
    assert_int_equal(result, -1);

    assert_true(eval_int_expr("+(-1)", &result));
    assert_int_equal(result, -1);

    assert_true(eval_int_expr("-(-1)", &result));
    assert_int_equal(result, 1);

    assert_true(eval_int_expr("~0xAB", &result));
    assert_int_equal((uint8_t)result, 0x54);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_compile_int_constants),
            cmocka_unit_test(test_compile_binary_exprs),
            cmocka_unit_test(test_compile_unary_exprs),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
