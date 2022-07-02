//
// Created by carlos on 6/25/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "parser.h"

static void test_(void **state) {
    (void) state;
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}