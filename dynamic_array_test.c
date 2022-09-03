//
// Created by carlos on 9/3/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "common.h"
#include "dynamic_array.h"

static void test_getset_ok(void **state) {
    UNUSED(state);

    struct test_type {
        int a;
        int b;
    };
    struct bee_dynamic_array *array = bee_dynamic_array_new(sizeof(struct test_type), 2, 2, NULL);
    assert_non_null(array);

    struct test_type s0 = {0x0A, 0x0B};
    assert_int_equal(bee_dynamic_array_set(array, 0, &s0), BEE_DA_STATUS_OK);
    assert_int_equal(array->elem_cap, 2);

    struct test_type s1 = {0x1A, 0x1B};
    assert_int_equal(bee_dynamic_array_set(array, 1, &s1), BEE_DA_STATUS_OK);
    assert_int_equal(array->elem_cap, 2);

    struct test_type g0 = {0};
    assert_int_equal(bee_dynamic_array_get(array, 0, &g0), BEE_DA_STATUS_OK);
    assert_int_equal(s0.a, g0.a);
    assert_int_equal(s0.b, g0.b);

    struct test_type g1 = {0};
    assert_int_equal(bee_dynamic_array_get(array, 1, &g1), BEE_DA_STATUS_OK);
    assert_int_equal(s1.a, g1.a);
    assert_int_equal(s1.b, g1.b);
    bee_dynamic_array_free(array);
}

static void test_getset_fails(void **state) {
    UNUSED(state);

    struct bee_dynamic_array *array = bee_dynamic_array_new(sizeof(char), 2, 2, NULL);
    assert_non_null(array);

    char c = 0;
    assert_int_equal(bee_dynamic_array_get(array, 0, &c), BEE_DA_STATUS_OK);
    assert_int_equal(bee_dynamic_array_get(array, 1, &c), BEE_DA_STATUS_OK);
    assert_int_equal(bee_dynamic_array_get(array, 2, &c), BEE_DA_STATUS_INDEX_OUT_OF_RANGE);
    assert_int_equal(bee_dynamic_array_get(array, 3, &c), BEE_DA_STATUS_INDEX_OUT_OF_RANGE);
}

static void test_stack_ok(void **state) {
    UNUSED(state);

    struct bee_dynamic_array *array = bee_dynamic_array_new(sizeof(char), 5, 5, NULL);
    assert_non_null(array);

    int i=0;
    for (;i<5; i++) {
        char v = (char)('a' + i);
        assert_int_equal(bee_dynamic_array_push_back(array, &v), BEE_DA_STATUS_OK);
        assert_int_equal(array->elem_cap, 5);
    }

    for (;i<10; i++) {
        char v = (char)('a' + i);
        assert_int_equal(bee_dynamic_array_push_back(array, &v), BEE_DA_STATUS_OK);
        assert_int_equal(array->elem_cap, 10);
    }
    assert_string_equal(array->data, "abcdefghij");

    char str[10] = {0};
    for (int j=0;j<10;j++) {
        assert_int_equal(bee_dynamic_array_pop_back(array, str+j), BEE_DA_STATUS_OK);
    }
    assert_string_equal(str, "jihgfedcba");
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