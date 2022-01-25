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

    struct bee_error_list *list = bee_error_list_new();
    bee_error_list_push(list, 1, 2, "a message");
    bee_error_list_push(list, 1, 4, "format %d", 10);

    assert_non_null(list->head);
    assert_string_equal(list->head->msg, "a message");

    assert_non_null(list->tail);
    assert_string_equal(list->tail->msg, "format 10");

    bee_error_list_free(list);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_error_push),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}