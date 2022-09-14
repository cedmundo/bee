//
// Created by carlos on 9/3/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "common.h"

struct mock_type { int a; int b; };

static void test_release(void *maybe_mock_type) {
    assert_non_null(maybe_mock_type);
    struct mock_type *mock_value = (struct mock_type *) maybe_mock_type;
    assert_int_equal(mock_value->a, 0xA);
    assert_int_equal(mock_value->b, 0xB);
    assert_int_equal(bee_rc_get(mock_value), 0);
}

static void test_rc_alloc(void **state) {
    UNUSED(state);
    struct mock_type *mock_value = bee_rc_alloc(sizeof(struct mock_type), test_release);
    assert_non_null(mock_value);
    assert_int_equal(bee_rc_get(mock_value), 1);

    mock_value->a = 0xA;
    mock_value->b = 0xB;

    struct mock_type *mock_ref = bee_rc_inc(mock_value);
    assert_int_equal(bee_rc_get(mock_value), 2);
    assert_int_equal(bee_rc_get(mock_ref), 2);

    assert_int_equal(mock_ref->a, 0xA);
    assert_int_equal(mock_ref->b, 0xB);

    bee_rc_dec(mock_ref);
    assert_int_equal(bee_rc_get(mock_value), 1);

    bee_rc_dec(mock_value);
}

static void test_rc_realloc(void **state) {
    UNUSED(state);
    struct mock_type *mock_value = bee_rc_alloc(sizeof(struct mock_type), NULL);
    assert_non_null(mock_value);
    assert_int_equal(bee_rc_get(mock_value), 1);

    mock_value->a = 0xA;
    mock_value->b = 0xB;

    struct mock_type *mock_two = bee_rc_realloc(mock_value, sizeof(struct mock_type) * 2);
    assert_non_null(mock_two);
    bee_rc_dec(mock_two);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_rc_alloc),
            cmocka_unit_test(test_rc_realloc),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}