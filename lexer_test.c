//
// Created by carlos on 1/19/22.
//
#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include "lexer.h"

static void test_is_space(void **state) {
    (void) state;

    assert_true(is_space(' '));
    assert_true(is_space('\t'));
    assert_false(is_space('x'));
}

static void test_is_newline(void **state) {
    (void) state;

    assert_true(is_eol('\n'));
    assert_false(is_eol('a'));
}

static void test_is_punct(void **state) {
    (void) state;

    assert_true(is_punct('+'));
    assert_true(is_punct('-'));
    assert_true(is_punct('*'));
    assert_true(is_punct('/'));
    assert_true(is_punct('%'));
    assert_true(is_punct('&'));
    assert_true(is_punct('|'));
    assert_true(is_punct('!'));
    assert_true(is_punct(':'));
    assert_true(is_punct('.'));
    assert_true(is_punct(','));
    assert_true(is_punct('='));
    assert_true(is_punct('>'));
    assert_true(is_punct('<'));
    assert_true(is_punct('('));
    assert_true(is_punct(')'));
    assert_true(is_punct('['));
    assert_true(is_punct(']'));
    assert_true(is_punct('{'));
    assert_true(is_punct('}'));
    assert_true(is_punct('?'));
    assert_true(is_punct('~'));
    assert_true(is_punct('\''));
    assert_true(is_punct('"'));
    assert_true(is_punct('@'));
    assert_false(is_punct(' '));
    assert_false(is_punct('a'));
    assert_false(is_punct('1'));
}

static void test_is_alpha(void **state) {
    (void) state;
    for (int c='a'; c<='z'; c++) {
        assert_true(is_alpha(c));
    }

    for (int c='A'; c<='Z'; c++) {
        assert_true(is_alpha(c));
    }

    assert_true(is_alpha(0x03A0)); // Π
    assert_false(is_alpha('0'));
    assert_false(is_alpha('9'));
}

static void test_is_digit(void **state) {
    (void) state;

    for (int c='0'; c<='9'; c++) {
        assert_true(is_digit(c));
    }

    assert_false(is_digit('.'));
    assert_false(is_digit('a'));
    assert_false(is_digit(' '));
}

static void test_is_alphanum(void **state) {
    (void) state;

    for (int c='a'; c<='z'; c++) {
        assert_true(is_alphanum(c));
    }

    for (int c='A'; c<='Z'; c++) {
        assert_true(is_alphanum(c));
    }

    for (int c='0'; c<='9'; c++) {
        assert_true(is_alphanum(c));
    }

    assert_true(is_alphanum('_'));
    assert_true(is_alphanum('$'));

    assert_false(is_alphanum(' '));
    assert_false(is_alphanum('\n'));
    assert_false(is_alphanum('.'));
}

static void test_is_keyword(void **state) {
    (void) state;

    assert_true(is_keyword("proc", 4));
    assert_true(is_keyword("end", 3));
    assert_true(is_keyword("in", 2));

    assert_false(is_keyword("procedure", 9));
    assert_false(is_keyword("inside", 6));
    assert_false(is_keyword("friend", 6));
}

static void test_tokenize_buffer_eof(void **state) {
    (void) state;

    struct bee_token *token_start = bee_token_new();
    bee_tokenize_buffer("", 0, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_EOF);
    bee_token_free(token_start);
}

static void test_tokenize_buffer_spaces(void **state) {
    (void) state;

    struct bee_token *token_start = bee_token_new();
    bee_tokenize_buffer("  \t\t", 4, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_EOF);
    bee_token_free(token_start);
}

static void test_tokenize_buffer_newlines(void **state) {
    (void) state;

    struct bee_token *token_start = bee_token_new();
    bee_tokenize_buffer("\n\n\n", 3, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_EOL);
    assert_int_equal(token_start->row, 1);
    assert_int_equal(token_start->col, 1);

    struct bee_token *second = token_start->next;
    assert_non_null(second);
    assert_int_equal(second->token_type, BEE_TT_EOF);
    assert_int_equal(second->row, 3);
    assert_int_equal(second->col, 0);
    bee_token_free(token_start);
}

static void test_tokenize_buffer_digits(void **state) {
    (void) state;

    struct bee_token *token_start = bee_token_new();
    bee_tokenize_buffer("1234 1234 1234", 5, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_DIGITS);
    assert_int_equal(token_start->len, 4);
    assert_memory_equal("1234", token_start->base, token_start->len);
    bee_token_free(token_start);

    token_start = bee_token_new();
    bee_tokenize_buffer("1234 4321", 9, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_DIGITS);
    assert_int_equal(token_start->len, 4);
    assert_memory_equal("1234", token_start->base, token_start->len);

    struct bee_token *second = token_start->next;
    assert_non_null(second);
    assert_int_equal(second->token_type, BEE_TT_DIGITS);
    assert_int_equal(second->len, 4);
    assert_memory_equal("4321", second->base, second->len);

    bee_token_free(token_start);
}

static void test_tokenize_buffer_strings(void **state) {
    (void) state;

    struct bee_token *token_start;
    struct bee_error *error_start;

    token_start = bee_token_new();
    error_start = bee_error_new();
    bee_tokenize_buffer("\"a simple string\"", 17, token_start, error_start);
    assert_null(error_start->msg);
    assert_int_equal(token_start->token_type, BEE_TT_STRING);
    assert_memory_equal("\"a simple string\"", token_start->base, token_start->len);
    bee_token_free(token_start);
    bee_error_free(error_start);

    token_start = bee_token_new();
    error_start = bee_error_new();
    bee_tokenize_buffer("\"a\" \"bb\"", 8, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_STRING);
    assert_memory_equal("\"a\"", token_start->base, token_start->len);

    struct bee_token *second = token_start->next;
    assert_non_null(second);
    assert_int_equal(second->token_type, BEE_TT_STRING);
    assert_memory_equal("\"bb\"", second->base, second->len);
    bee_token_free(token_start);
    bee_error_free(error_start);

    token_start = bee_token_new();
    error_start = bee_error_new();
    bee_tokenize_buffer("\"a \\\"\"", 6, token_start, error_start);
    assert_null(error_start->msg);
    assert_int_equal(token_start->token_type, BEE_TT_STRING);
    assert_memory_equal("\"a \\\"\"", token_start->base, token_start->len);
    bee_token_free(token_start);
    bee_error_free(error_start);

    token_start = bee_token_new();
    error_start = bee_error_new();
    bee_tokenize_buffer("\"nope", 5, token_start, error_start);
    assert_string_equal("unterminated string", error_start->msg);
    bee_token_free(token_start);
    bee_error_free(error_start);

    token_start = bee_token_new();
    error_start = bee_error_new();
    bee_tokenize_buffer("\"nope\nnope\"", 11, token_start, error_start);
    assert_string_equal("unterminated string", error_start->msg);
    bee_token_free(token_start);
    bee_error_free(error_start);

    token_start = bee_token_new();
    error_start = bee_error_new();
    bee_tokenize_buffer("\"nope\\\"", 7, token_start, error_start);
    assert_string_equal("unterminated string", error_start->msg);
    bee_token_free(token_start);
    bee_error_free(error_start);
}

static void test_tokenize_buffer_words_and_keywords(void **state) {
    (void) state;

    struct bee_token *token_start;

    token_start = bee_token_new();
    bee_tokenize_buffer("a word", 6, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_WORD);
    assert_int_equal(token_start->len, 1);
    assert_memory_equal("a", token_start->base, token_start->len);
    bee_token_free(token_start);

    token_start = bee_token_new();
    bee_tokenize_buffer("proc", 4, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_KEYWORD);
    assert_int_equal(token_start->len, 4);
    bee_token_free(token_start);

    token_start = bee_token_new();
    bee_tokenize_buffer("procedure", 9, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_WORD);
    assert_int_equal(token_start->len, 9);
    bee_token_free(token_start);

    token_start = bee_token_new();
    bee_tokenize_buffer("friend", 6, token_start, NULL);
    assert_int_equal(token_start->token_type, BEE_TT_WORD);
    assert_int_equal(token_start->len, 6);
    bee_token_free(token_start);
}

static void test_tokenize_buffer_puncts(void **state) {
    (void) state;
    struct bee_token *token_start;

    token_start = bee_token_new();
    bee_tokenize_buffer("=+()", 4, token_start, NULL);
    uint8_t punct_count = 0L;
    struct bee_token *cur_token = token_start;
    while (cur_token != NULL) {
        if (cur_token->token_type != BEE_TT_EOF) {
            assert_int_equal(cur_token->token_type, BEE_TT_PUNCT);
            assert_int_equal(cur_token->len, 1);
            punct_count++;
        } else {
            assert_ptr_not_equal(cur_token, token_start);
        }
        cur_token = cur_token->next;
    }
    assert_int_equal(punct_count, 4);
    bee_token_free(token_start);
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
            cmocka_unit_test(test_tokenize_buffer_eof),
            cmocka_unit_test(test_tokenize_buffer_spaces),
            cmocka_unit_test(test_tokenize_buffer_newlines),
            cmocka_unit_test(test_tokenize_buffer_digits),
            cmocka_unit_test(test_tokenize_buffer_strings),
            cmocka_unit_test(test_tokenize_buffer_words_and_keywords),
            cmocka_unit_test(test_tokenize_buffer_puncts),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}