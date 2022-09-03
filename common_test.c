//
// Created by carlos on 9/3/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "common.h"

static void test_dummy(void **state) {
    UNUSED(state);
    assert_true(1);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_dummy),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}