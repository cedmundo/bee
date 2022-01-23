//
// Created by carlos on 1/19/22.
//
#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include "parser.h"

static void test_parser_dummy(void **state) {
    (void) state;
    assert_int_equal(1, 1);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parser_dummy),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}