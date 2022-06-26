//
// Created by carlos on 6/10/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "lexer.h"

static void test_tokenize_num(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "123");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.base + token.off, "123", token.len);

    token = bee_token_start("test_tokenize", "12.12");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.base + token.off, "12.12", token.len);

    token = bee_token_start("test_tokenize", "12 21");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.base + token.off, "12", token.len);

    token = bee_token_next(token);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.base + token.off, "21", token.len);
}

static void test_tokenize_str(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "\"a string\"");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 10);
    assert_memory_equal(token.base + token.off, "\"a string\"", token.len);

    token = bee_token_start("test_tokenize", "\"\"");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.base + token.off, "\"\"", token.len);

    token = bee_token_start("test_tokenize", "\"\\\"\"");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 4);
    assert_memory_equal(token.base + token.off, "\"\\\"\"", token.len);

    token = bee_token_start("test_tokenize", "\"not terminated");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_UNTERMINATED_STRING);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);

    token = bee_token_start("test_tokenize", "\"a\"\"b\"");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.base + token.off, "\"a\"", token.len);

    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.base + token.off, "\"b\"", token.len);
}

static void test_tokenize_punct(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "+ - * /");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.base + token.off, "+", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_ADD);

    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.base + token.off, "-", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_SUB);

    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.base + token.off, "*", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_MUL);

    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.base + token.off, "/", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_DIV);

    token = bee_token_start("test_tokenize", "===");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.base + token.off, "==", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_EQ);

    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.base + token.off, "=", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_ASSIGN);
}

static void test_tokenize_keyword(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "function");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_KEYWORD);
    assert_int_equal(token.len, 8);
    assert_memory_equal(token.base + token.off, "function", token.len);
    assert_int_equal(token.keyword_type, BEE_KEYWORD_FUNCTION);

    token = bee_token_start("test_tokenize", "tend");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_not_equal(token.type, BEE_TOKEN_TYPE_KEYWORD);

    token = bee_token_start("test_tokenize", "ended");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_not_equal(token.type, BEE_TOKEN_TYPE_KEYWORD);
}

static void test_tokenize_id(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "a_name");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 6);
    assert_memory_equal(token.base + token.off, "a_name", token.len);

    token = bee_token_start("test_tokenize", "a");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.base + token.off, "a", token.len);

    token = bee_token_start("test_tokenize", "_a");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.base + token.off, "_a", token.len);

    token = bee_token_start("test_tokenize", "$a");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.base + token.off, "$a", token.len);
}

static void test_tokenize_bol(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "true");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_BOOLEAN);
    assert_int_equal(token.len, 4);
    assert_memory_equal(token.base + token.off, "true", token.len);

    token = bee_token_start("test_tokenize", "false");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_BOOLEAN);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.base + token.off, "false", token.len);

    token = bee_token_start("test_tokenize", "trued");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.base + token.off, "trued", token.len);

    token = bee_token_start("test_tokenize", "_true");
    token = bee_token_next(token);
    assert_int_equal(token.error, BEE_TOKEN_ERROR_NONE);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.base + token.off, "_true", token.len);
}

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_tokenize_num),
            cmocka_unit_test(test_tokenize_str),
            cmocka_unit_test(test_tokenize_punct),
            cmocka_unit_test(test_tokenize_keyword),
            cmocka_unit_test(test_tokenize_id),
            cmocka_unit_test(test_tokenize_bol),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}