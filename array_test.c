//
// Created by carlos on 9/3/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "common.h"
#include "array.h"

struct mock_type { int a; int b; };

static void test_type_release(void *maybe_test_type) {
    assert_non_null(maybe_test_type);
}

static struct mock_type *mock_type_new(int a, int b) {
    struct mock_type *mock_value = bee_rc_alloc(sizeof(struct mock_type), test_type_release);
    mock_value->a = a;
    mock_value->b = b;
    return mock_value;
}

static void test_getset_ok(void **state) {
    UNUSED(state);
    struct bee_array *array = bee_array_new(2, 2);
    assert_non_null(array);

    struct mock_type *s0 = mock_type_new(0x0A, 0x0B);
    assert_int_equal(bee_array_set(array, 0, s0), BEE_OP_RES_OK);
    assert_int_equal(array->cap, 2);
    assert_int_equal(bee_rc_get(s0), 2);

    struct mock_type *s1 = mock_type_new(0x1A, 0x1B);
    assert_int_equal(bee_array_set(array, 1, s1), BEE_OP_RES_OK);
    assert_int_equal(array->cap, 2);
    assert_int_equal(bee_rc_get(s0), 2);

    struct mock_type *g0 = NULL;
    assert_int_equal(bee_array_get(array, 0, into_cell(g0)), BEE_OP_RES_OK);
    assert_int_equal(s0->a, g0->a);
    assert_int_equal(s0->b, g0->b);

    struct mock_type *g1 = NULL;
    assert_int_equal(bee_array_get(array, 1, into_cell(g1)), BEE_OP_RES_OK);
    assert_int_equal(s1->a, g1->a);
    assert_int_equal(s1->b, g1->b);

    bee_rc_dec(s0);
    bee_rc_dec(s1);
    bee_rc_dec(array);
}

static void test_getset_fails(void **state) {
    UNUSED(state);

    struct bee_array *array = bee_array_new(2, 2);
    assert_non_null(array);

    char *c = NULL;
    assert_int_equal(bee_array_get(array, 0, into_cell(c)), BEE_OP_RES_OK);
    assert_int_equal(bee_array_get(array, 1, into_cell(c)), BEE_OP_RES_OK);
    assert_int_equal(bee_array_get(array, 2, into_cell(c)), BEE_OP_RES_INDEX_OUT_OF_RANGE);
    assert_int_equal(bee_array_get(array, 3, into_cell(c)), BEE_OP_RES_INDEX_OUT_OF_RANGE);
    bee_rc_dec(array);
}

static void test_stack_ok(void **state) {
    UNUSED(state);

    struct bee_array *array = bee_array_new(5, 5);
    assert_non_null(array);

    for (int i=0;i<5;i++) {
        struct mock_type *elem = mock_type_new(0xA, i);
        assert_int_equal(bee_array_push_back(array, elem), BEE_OP_RES_OK);

        // Array should not increase it's size if not needed
        assert_int_equal(bee_array_len(array), i+1);
        assert_int_equal(bee_array_cap(array), 5);

        // Array should reference children elements
        assert_int_equal(bee_rc_get(elem), 2);
        bee_rc_dec(elem);
    }

    for (int i=0;i<5;i++) {
        struct mock_type *elem = NULL;

        // Should be able to get the i-th ref into elem cell, its value must be a=0xA, b=i
        assert_int_equal(bee_array_get(array, i, into_cell(elem)), BEE_OP_RES_OK);
        assert_non_null(elem);
        assert_int_equal(elem->a, 0xA);
        assert_int_equal(elem->b, i);

        // The only reference should be the array itself
        assert_int_equal(bee_rc_get(elem), 1);
    }

    for (int i=0;i<5;i++) {
        struct mock_type *elem = mock_type_new(i, 0xB);
        assert_int_equal(bee_array_push_back(array, elem), BEE_OP_RES_OK);

        // Array should increase its size if needed
        assert_int_equal(bee_array_len(array), i+6);
        assert_int_equal(bee_array_cap(array), 10);

        // Array should reference children elements
        assert_int_equal(bee_rc_get(elem), 2);
        bee_rc_dec(elem);
    }

    for (int i=0;i<5;i++) {
        struct mock_type *elem = NULL;

        // Array should pop from back
        assert_int_equal(bee_array_pop_back(array, into_cell(elem)), BEE_OP_RES_OK);
        assert_non_null(elem);
        assert_int_equal(elem->a, 4-i);
        assert_int_equal(elem->b, 0xB);

        // The only reference should be the array itself
        assert_int_equal(bee_rc_get(elem), 1);
        bee_rc_dec(elem);
    }

    bee_rc_dec(array);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_getset_ok),
            cmocka_unit_test(test_getset_fails),
            cmocka_unit_test(test_stack_ok),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}