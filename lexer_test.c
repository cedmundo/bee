//
// Created by carlos on 1/19/22.
//
#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include "lexer.h"
#include "error.h"

static void test_is_space(void **state) {
    (void) state;

    assert_true(bee_is_space(' '));
    assert_true(bee_is_space('\t'));
    assert_false(bee_is_space('x'));
}

static void test_is_newline(void **state) {
    (void) state;

    assert_true(bee_is_eol('\n'));
    assert_false(bee_is_eol('a'));
}

static void test_is_punct(void **state) {
    (void) state;

    assert_true(bee_is_punct('+'));
    assert_true(bee_is_punct('-'));
    assert_true(bee_is_punct('*'));
    assert_true(bee_is_punct('/'));
    assert_true(bee_is_punct('%'));
    assert_true(bee_is_punct('&'));
    assert_true(bee_is_punct('|'));
    assert_true(bee_is_punct('!'));
    assert_true(bee_is_punct(':'));
    assert_true(bee_is_punct('.'));
    assert_true(bee_is_punct(','));
    assert_true(bee_is_punct('='));
    assert_true(bee_is_punct('>'));
    assert_true(bee_is_punct('<'));
    assert_true(bee_is_punct('('));
    assert_true(bee_is_punct(')'));
    assert_true(bee_is_punct('['));
    assert_true(bee_is_punct(']'));
    assert_true(bee_is_punct('{'));
    assert_true(bee_is_punct('}'));
    assert_true(bee_is_punct('?'));
    assert_true(bee_is_punct('~'));
    assert_true(bee_is_punct('\''));
    assert_true(bee_is_punct('"'));
    assert_true(bee_is_punct('@'));
    assert_false(bee_is_punct(' '));
    assert_false(bee_is_punct('a'));
    assert_false(bee_is_punct('1'));
}

static void test_is_alpha(void **state) {
    (void) state;
    for (int c='a'; c<='z'; c++) {
        assert_true(bee_is_alpha(c));
    }

    for (int c='A'; c<='Z'; c++) {
        assert_true(bee_is_alpha(c));
    }

    assert_true(bee_is_alpha(0x03A0)); // Π
    assert_false(bee_is_alpha('0'));
    assert_false(bee_is_alpha('9'));
}

static void test_is_digit(void **state) {
    (void) state;

    for (int c='0'; c<='9'; c++) {
        assert_true(bee_is_digit(c));
    }

    assert_false(bee_is_digit('.'));
    assert_false(bee_is_digit('a'));
    assert_false(bee_is_digit(' '));
}

static void test_is_alphanum(void **state) {
    (void) state;

    for (int c='a'; c<='z'; c++) {
        assert_true(bee_is_alphanum(c));
    }

    for (int c='A'; c<='Z'; c++) {
        assert_true(bee_is_alphanum(c));
    }

    for (int c='0'; c<='9'; c++) {
        assert_true(bee_is_alphanum(c));
    }

    assert_true(bee_is_alphanum('_'));
    assert_true(bee_is_alphanum('$'));

    assert_false(bee_is_alphanum(' '));
    assert_false(bee_is_alphanum('\n'));
    assert_false(bee_is_alphanum('.'));
}

static void test_is_keyword(void **state) {
    (void) state;

    assert_true(bee_is_keyword("proc", 4));
    assert_true(bee_is_keyword("end", 3));
    assert_true(bee_is_keyword("in", 2));

    assert_false(bee_is_keyword("procedure", 9));
    assert_false(bee_is_keyword("inside", 6));
    assert_false(bee_is_keyword("friend", 6));
}

static void test_token_consume_any(void **state) {
    (void) state;

    struct bee_error_list *errors = bee_error_list_new();
    struct bee_token token = bee_token_start("const example = \"string\" 10", errors);

    token = bee_token_consume_any(token);
    assert_int_equal(token.token_type, BEE_TT_KEYWORD);
    assert_memory_equal("const", token.base, token.len);

    token = bee_token_consume_any(token);
    assert_int_equal(token.token_type, BEE_TT_WORD);
    assert_memory_equal("example", token.base, token.len);

    token = bee_token_consume_any(token);
    assert_int_equal(token.token_type, BEE_TT_PUNCT);
    assert_memory_equal("=", token.base, token.len);

    token = bee_token_consume_any(token);
    assert_int_equal(token.token_type, BEE_TT_STRING);
    assert_memory_equal("\"string\"", token.base, token.len);

    token = bee_token_consume_any(token);
    assert_int_equal(token.token_type, BEE_TT_DIGITS);
    assert_memory_equal("10", token.base, token.len);

    bee_error_list_free(errors);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_is_space),
            cmocka_unit_test(test_is_newline),
            cmocka_unit_test(test_is_punct),
            cmocka_unit_test(test_is_alpha),
            cmocka_unit_test(test_is_digit),
            cmocka_unit_test(test_is_alphanum),
            cmocka_unit_test(test_is_keyword),
            cmocka_unit_test(test_token_consume_any),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}