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
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.data + token.off, "123", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_start("test_tokenize", "12.12");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data + token.off, "12.12", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_start("test_tokenize", "12 21");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data + token.off, "12", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_next(token);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data + token.off, "21", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 3);

    token = bee_token_start("test_tokenize", "-10");
    token = bee_token_next(token);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.data + token.off, "-10", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_start("test_tokenize", "10.10");
    token = bee_token_next(token);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data + token.off, "10.10", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_start("test_tokenize", "10.10_f32");
    token = bee_token_next(token);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 9);
    assert_memory_equal(token.data + token.off, "10.10_f32", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_start("test_tokenize", "0xFA");
    token = bee_token_next(token);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 4);
    assert_memory_equal(token.data + token.off, "0xFA", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_start("test_tokenize", "-0xFA");
    token = bee_token_next(token);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data + token.off, "-0xFA", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_start("test_tokenize", "-0b10_u8");
    token = bee_token_next(token);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_NUMBER);
    assert_int_equal(token.len, 8);
    assert_memory_equal(token.data + token.off, "-0b10_u8", token.len);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);
}

static void test_tokenize_str(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "\"a string\"");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 10);
    assert_memory_equal(token.data + token.off, "\"a string\"", token.len);

    token = bee_token_start("test_tokenize", "\"\"");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data + token.off, "\"\"", token.len);

    token = bee_token_start("test_tokenize", "\"\\\"\"");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 4);
    assert_memory_equal(token.data + token.off, "\"\\\"\"", token.len);

    token = bee_token_start("test_tokenize", "\"not terminated");
    token = bee_token_next(token);
    assert_true(bee_has_error(&token.error));
    assert_int_equal(token.error.type, BEE_ERROR_LEXER_UNTERMINATED_STR);
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);

    token = bee_token_start("test_tokenize", "\"a\"\"b\"");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.data + token.off, "\"a\"", token.len);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_STRING);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.data + token.off, "\"b\"", token.len);
}

static void test_tokenize_punct(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "+ - * /");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data + token.off, "+", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_PLUS);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data + token.off, "-", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_MINUS);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data + token.off, "*", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_ASTERISK);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data + token.off, "/", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_SLASH);

    token = bee_token_start("test_tokenize", ">=>");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data + token.off, ">=", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_GE);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data + token.off, ">", token.len);
    assert_int_equal(token.punct_type, BEE_PUNCT_GT);
}

static void test_tokenize_keyword(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "not");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_KEYWORD);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.data + token.off, "not", token.len);
    assert_int_equal(token.keyword_type, BEE_KEYWORD_NOT);

    token = bee_token_start("test_tokenize", "note");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_not_equal(token.type, BEE_TOKEN_TYPE_KEYWORD);

    token = bee_token_start("test_tokenize", "snot");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_not_equal(token.type, BEE_TOKEN_TYPE_KEYWORD);
}

static void test_tokenize_id(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "a_name");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 6);
    assert_memory_equal(token.data + token.off, "a_name", token.len);

    token = bee_token_start("test_tokenize", "a");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data + token.off, "a", token.len);

    token = bee_token_start("test_tokenize", "_a");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data + token.off, "_a", token.len);

    token = bee_token_start("test_tokenize", "$a");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data + token.off, "$a", token.len);
}

static void test_tokenize_bol(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "true");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_BOOLEAN);
    assert_int_equal(token.len, 4);
    assert_memory_equal(token.data + token.off, "true", token.len);

    token = bee_token_start("test_tokenize", "false");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_BOOLEAN);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data + token.off, "false", token.len);

    token = bee_token_start("test_tokenize", "trued");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data + token.off, "trued", token.len);

    token = bee_token_start("test_tokenize", "_true");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data + token.off, "_true", token.len);
}

static void test_tokenize_spaces(void **state) {
    (void) state;
    struct bee_token token;

    token = bee_token_start("test_tokenize", "    a   b");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 4);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 8);

    token = bee_token_start("test_tokenize", "a\nb");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_EOL);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 1);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.row, 2);
    assert_int_equal(token.col, 0);

    token = bee_token_start("test_tokenize", "a\n\n\nb");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_EOL);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 1);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.row, 4);
    assert_int_equal(token.col, 0);

    token = bee_token_start("test_tokenize", "a\n  \n  \nb");
    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 0);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_EOL);
    assert_int_equal(token.row, 1);
    assert_int_equal(token.col, 1);

    token = bee_token_next(token);
    assert_false(bee_has_error(&token.error));
    assert_int_equal(token.type, BEE_TOKEN_TYPE_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.row, 4);
    assert_int_equal(token.col, 0);
}

int main() {
    // NOTE: for unused imports
    (void) (jmp_buf *) 0;
    (void) (va_list *) 0;

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_tokenize_num),
            cmocka_unit_test(test_tokenize_str),
            cmocka_unit_test(test_tokenize_punct),
            cmocka_unit_test(test_tokenize_keyword),
            cmocka_unit_test(test_tokenize_id),
            cmocka_unit_test(test_tokenize_bol),
            cmocka_unit_test(test_tokenize_spaces),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}