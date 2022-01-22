//
// Created by carlos on 1/19/22.
//
#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include "lexer.h"

static void test_token_push(void **state) {
    (void) state;
    struct bee_token *first = bee_token_new();
    assert_non_null(first);

    struct bee_token *another = bee_token_push(first, "word", 4, 1, 1, BEE_TT_PUNCT);
    assert_non_null(another);

    struct bee_token *last = bee_token_push(another, "other", 5, 1, 1, BEE_TT_EOF);
    assert_null(last);

    struct bee_token *cur = first;
    while (cur != NULL) {
        assert_non_null(cur);
        cur = cur->next;
    }

    bee_token_free(first);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_token_push),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}