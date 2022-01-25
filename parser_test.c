//
// Created by carlos on 1/19/22.
//
#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <jit/jit-util.h>
#include "parser.h"
#include "lexer.h"

static void test_parse_module(void **state) {
    (void) state;

    char *code_d = "struct User; var username: String; var password: String; end struct User; ";
    size_t code_s = jit_strlen(code_d);

    struct bee_token *token_start = bee_token_new();
    bee_tokenize_buffer(code_d, code_s, token_start, NULL);
    bee_parse(token_start, NULL);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_module),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}