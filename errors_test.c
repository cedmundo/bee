//
// Created by carlos on 9/3/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "errors.h"

static void test_error_set(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    bee_error_init(&error);
    assert_false(bee_error_is_set(&error));

    bee_error_set(&error, (struct bee_loc){.filename="test_error_set", .row=1, .col=0}, "an error");
    assert_true(bee_error_is_set(&error));
}

static void test_error_print(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    bee_error_set(&error, (struct bee_loc){.filename="test_error_set", .row=1, .col=0}, "safe error, totally expected");
    bee_error_print(&error);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_error_set),
            cmocka_unit_test(test_error_print),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}