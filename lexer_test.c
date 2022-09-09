//
// Created by carlos on 9/3/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "lexer.h"

static void test_tokenize_num(void **state) {
    (void) state;
    struct bee_token token;
    struct bee_error error = {0};

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "123");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.data, "123", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_DEC);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_U32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "12.12");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data, "12.12", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_DEC);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_F32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "12 21");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data, "12", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_DEC);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_U32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data, "21", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_DEC);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_U32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 3);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "-10");
    token = bee_token_next(token, &error);
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.data, "-10", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_DEC);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_I32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "10.10");
    token = bee_token_next(token, &error);
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data, "10.10", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_DEC);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_F32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "10.10_f32");
    token = bee_token_next(token, &error);
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 9);
    assert_memory_equal(token.data, "10.10_f32", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_DEC);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_F32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "0xFA");
    token = bee_token_next(token, &error);
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 4);
    assert_memory_equal(token.data, "0xFA", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_HEX);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_U32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "-0xFA");
    token = bee_token_next(token, &error);
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data, "-0xFA", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_HEX);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_I32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "-0b10_u8");
    token = bee_token_next(token, &error);
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 8);
    assert_memory_equal(token.data, "-0b10_u8", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_BIN);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_U8);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "0b99");
    bee_token_next(token, &error);
    assert_true(bee_error_is_set(&error));
    assert_string_equal(error.msg, "digit `9` is outside number base");

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "0-1");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NUMBER);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data, "0", token.len);
    assert_int_equal(token.num_base, BEE_NUM_BASE_DEC);
    assert_int_equal(token.num_type, BEE_NUM_TYPE_U32);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);
}

static void test_tokenize_str(void **state) {
    (void) state;
    struct bee_token token;
    struct bee_error error = {0};

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "\"a string\"");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_STRING);
    assert_int_equal(token.len, 10);
    assert_memory_equal(token.data, "\"a string\"", token.len);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "\"\"");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_STRING);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data, "\"\"", token.len);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "\"\\\"\"");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_STRING);
    assert_int_equal(token.len, 4);
    assert_memory_equal(token.data, "\"\\\"\"", token.len);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "\"not terminated");
    token = bee_token_next(token, &error);
    assert_true(bee_error_is_set(&error));
    assert_string_equal(error.msg, "unterminated string");
    assert_int_equal(token.tag, BEE_TOKEN_TAG_NONE);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "\"a\"\"b\"");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_STRING);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.data, "\"a\"", token.len);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_STRING);
    assert_int_equal(token.len, 3);
    assert_memory_equal(token.data, "\"b\"", token.len);
}

static void test_tokenize_punct(void **state) {
    (void) state;
    struct bee_token token;
    struct bee_error error = {0};

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "+ - * /");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data, "+", token.len);
    assert_int_equal(token.punct_tag, BEE_PUNCT_PLUS);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data, "-", token.len);
    assert_int_equal(token.punct_tag, BEE_PUNCT_MINUS);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data, "*", token.len);
    assert_int_equal(token.punct_tag, BEE_PUNCT_ASTERISK);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data, "/", token.len);
    assert_int_equal(token.punct_tag, BEE_PUNCT_SLASH);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", ">=>");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_PUNCT);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data, ">=", token.len);
    assert_int_equal(token.punct_tag, BEE_PUNCT_GE);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_PUNCT);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data, ">", token.len);
    assert_int_equal(token.punct_tag, BEE_PUNCT_GT);
}

static void test_tokenize_keyword(void **state) {
    (void) state;
    struct bee_token token;
    struct bee_error error = {0};

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "assert");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_KEYWORD);
    assert_int_equal(token.len, 6);
    assert_memory_equal(token.data, "assert", token.len);
    assert_int_equal(token.keyword_tag, BEE_KEYWORD_ASSERT);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "assertion");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_not_equal(token.tag, BEE_TOKEN_TAG_KEYWORD);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "_assert");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_not_equal(token.tag, BEE_TOKEN_TAG_KEYWORD);
}

static void test_tokenize_id(void **state) {
    (void) state;
    struct bee_token token;
    struct bee_error error = {0};

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "a_name");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 6);
    assert_memory_equal(token.data, "a_name", token.len);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "a");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 1);
    assert_memory_equal(token.data, "a", token.len);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "_a");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data, "_a", token.len);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "$a");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 2);
    assert_memory_equal(token.data, "$a", token.len);
}

static void test_tokenize_bol(void **state) {
    (void) state;
    struct bee_token token;
    struct bee_error error = {0};

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "true");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_BOOLEAN);
    assert_int_equal(token.len, 4);
    assert_memory_equal(token.data, "true", token.len);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "false");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_BOOLEAN);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data, "false", token.len);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "trued");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data, "trued", token.len);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "_true");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 5);
    assert_memory_equal(token.data, "_true", token.len);
}

static void test_tokenize_spaces(void **state) {
    (void) state;
    struct bee_token token;
    struct bee_error error = {0};

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "    a   b");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 4);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 8);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "a\nb");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_EOL);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 1);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.loc.row, 2);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "a\n\n\nb");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_EOL);
    assert_int_equal(token.len, 3);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 1);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.loc.row, 4);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_start("test_tokenize", "a\n  \n  \nb");
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 0);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_EOL);
    assert_int_equal(token.loc.row, 1);
    assert_int_equal(token.loc.col, 1);

    bee_error_clear(&error);
    token = bee_token_next(token, &error);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(token.tag, BEE_TOKEN_TAG_ID);
    assert_int_equal(token.len, 1);
    assert_int_equal(token.loc.row, 4);
    assert_int_equal(token.loc.col, 0);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

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