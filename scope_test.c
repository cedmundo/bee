//
// Created by carlos on 7/7/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "scope.h"
#include <jit/jit-function.h>
#include <jit/jit-context.h>
#include <jit/jit.h>

static void test_scope_bind_get(void **state) {
    (void) state;

    struct bee_scope *scope = bee_scope_new();

    jit_init();
    jit_context_t ctx = jit_context_create();
    jit_type_t s = jit_type_create_signature(jit_abi_cdecl, jit_type_void, NULL, 0, 0);
    jit_function_t f = jit_function_create(ctx, s);
    jit_value_t v = jit_value_create_nint_constant(f, jit_type_int, 0x1FA);
    union bee_object o = {.as_value = v};
    assert_true(bee_scope_bind(scope, "value", o));

    union bee_object r;
    assert_true(bee_scope_get(scope, "value", &r));
    assert_int_equal(jit_value_get_nint_constant(r.as_value), 0x1FA);

    jit_type_free(s);
    jit_function_abandon(f);
    jit_context_destroy(ctx);
    bee_scope_free(scope);
}

static void test_scope_fails_get_undefined(void **state) {
    (void) state;

    struct bee_scope *scope = bee_scope_new();
    assert_false(bee_scope_get(scope, "undefined", NULL));
    bee_scope_free(scope);
}

static void test_scope_fails_on_redefine(void **state) {
    (void) state;

    struct bee_scope *scope = bee_scope_new();

    jit_init();
    jit_context_t ctx = jit_context_create();
    jit_type_t s = jit_type_create_signature(jit_abi_cdecl, jit_type_void, NULL, 0, 0);
    jit_function_t f = jit_function_create(ctx, s);
    jit_value_t v = jit_value_create_nint_constant(f, jit_type_int, 0x1FA);
    union bee_object o = {.as_value = v};

    assert_true(bee_scope_bind(scope, "value", o));
    assert_false(bee_scope_bind(scope, "value", o));

    jit_type_free(s);
    jit_function_abandon(f);
    jit_context_destroy(ctx);
    bee_scope_free(scope);
}

static void test_scope_push_bind_and_pop(void **state) {
    (void) state;

    struct bee_scope *scope = bee_scope_new();

    jit_init();
    jit_context_t ctx = jit_context_create();
    jit_type_t s = jit_type_create_signature(jit_abi_cdecl, jit_type_void, NULL, 0, 0);
    jit_function_t f = jit_function_create(ctx, s);
    jit_value_t v0 = jit_value_create_nint_constant(f, jit_type_int, 0xA);
    jit_value_t v1 = jit_value_create_nint_constant(f, jit_type_int, 0xB);
    union bee_object o0 = {.as_value = v0};
    union bee_object o1 = {.as_value = v1};

    assert_true(bee_scope_bind(scope, "value", o0));
    bee_scope_push(scope);
    assert_true(bee_scope_bind(scope, "value", o1));

    union bee_object r;
    assert_true(bee_scope_get(scope, "value", &r));
    assert_int_equal(jit_value_get_nint_constant(r.as_value), 0xB);

    bee_scope_pop(scope);
    assert_true(bee_scope_get(scope, "value", &r));
    assert_int_equal(jit_value_get_nint_constant(r.as_value), 0xA);

    jit_type_free(s);
    jit_function_abandon(f);
    jit_context_destroy(ctx);
    bee_scope_free(scope);
}

int main() {
    // NOTE: for unused imports
    (void) (jmp_buf *) 0;
    (void) (va_list *) 0;

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_scope_bind_get),
            cmocka_unit_test(test_scope_fails_on_redefine),
            cmocka_unit_test(test_scope_fails_get_undefined),
            cmocka_unit_test(test_scope_push_bind_and_pop),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
