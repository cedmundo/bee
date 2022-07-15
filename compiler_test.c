//
// Created by carlos on 7/6/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include <jit/jit.h>
#include "parser.h"
#include "scope.h"
#include "compiler.h"

static int mul(int a, int b) {
    return a * b;
}

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
        fprintf(stderr, "%s:%ld:%ld: parse error: %s\n", token.name, token.row, token.col, p_error.msg);
        return false;
    }

    // built-in values
    struct bee_scope *scope = bee_scope_new();
    bee_scope_bind(scope, "zero", (union bee_object){
        .as_value=jit_value_create_nint_constant(function, jit_type_int, 0)
    });
    bee_scope_bind(scope, "one", (union bee_object){
        .as_value=jit_value_create_nint_constant(function, jit_type_int, 1)
    });
    bee_scope_bind(scope, "two", (union bee_object){
        .as_value=jit_value_create_nint_constant(function, jit_type_int, 2)
    });

    // built-in functions
    jit_type_t mul_params[] = {jit_type_int, jit_type_int};
    jit_type_t mul_sign = jit_type_create_signature(jit_abi_cdecl, jit_type_int, mul_params, 2, true);
    bee_scope_bind(scope, "mul", (union bee_object) {
        .as_func = {
            .signature = mul_sign,
            .flags = JIT_CALL_NOTHROW,
            .is_native = true,
            .native_addr = mul,
            .name = "mul",
        }
    });

    struct bee_compiler_error c_error = {.type = BEE_COMPILER_ERROR_NONE};
    union bee_object ret_value = bee_compile_node(function, node, &c_error, scope);
    if (c_error.type != BEE_COMPILER_ERROR_NONE) {
        fprintf(stderr, "%s:%ld:%ld: compile error: %s\n", c_error.filename, c_error.row, c_error.col, c_error.msg);
        return false;
    }

    jit_insn_return(function, ret_value.as_value);
    jit_function_compile(function);
    jit_context_build_end(context);

    // Apply function
    jit_function_apply(function, NULL, result);
    bee_ast_node_free(node);
    bee_scope_free(scope);
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

static void test_compile_id_expr(void **state) {
    (void) state;

    jit_int result = 0;
    assert_true(eval_int_expr("two * 3", &result));
    assert_int_equal(result, 6);

    assert_true(eval_int_expr("-one", &result));
    assert_int_equal(result, -1);

    assert_false(eval_int_expr("undefined + 1", &result));
}

static void test_compile_let_in(void **state) {
    (void) state;

    jit_int result = 0;
    assert_true(eval_int_expr("let x := 1 in x + 1", &result));
    assert_int_equal(result, 2);

    assert_true(eval_int_expr("let x := 2 in let xx := x * x in xx + 2 * x + 1", &result));
    assert_int_equal(result, 9);

    assert_true(eval_int_expr("let x := let y := 1 in y + 1 in x + 1", &result));
    assert_int_equal(result, 3);

    assert_false(eval_int_expr("let 1", &result));
    assert_false(eval_int_expr("let x := 1", &result));
    assert_false(eval_int_expr("let x := 1 in", &result));
    assert_false(eval_int_expr("let x := in x", &result));
    assert_false(eval_int_expr("let x := 1 in y", &result));
}

static void test_compile_call(void **state) {
    (void) state;

}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_compile_int_constants),
            cmocka_unit_test(test_compile_binary_exprs),
            cmocka_unit_test(test_compile_unary_exprs),
            cmocka_unit_test(test_compile_id_expr),
            cmocka_unit_test(test_compile_let_in),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
