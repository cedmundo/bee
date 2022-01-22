//
// Created by carlos on 1/19/22.
//
#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include "error.h"

static void test_error_push(void **state) {
    (void) state;
    struct bee_error *first = bee_error_new();
    assert_non_null(first);

    struct bee_error *second = bee_error_push(first, 1, 1, "message");
    assert_non_null(second);
    assert_string_equal(first->msg, "message");

    struct bee_error *last = bee_error_push(second, 1, 1, "format %d", 10);
    assert_non_null(last);
    assert_string_equal(second->msg, "format 10");

    bee_error_free(first);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_error_push),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}