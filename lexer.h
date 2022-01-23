//
// Created by carlos on 1/19/22.
//

#ifndef BEE_LEXER_H
#define BEE_LEXER_H

#include "lexer.h"
#include "error.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

enum bee_token_type {
    BEE_TT_EOF,
    BEE_TT_EOL,
    BEE_TT_PUNCT,
    BEE_TT_KEYWORD,
    BEE_TT_WORD,
    BEE_TT_STRING,
    BEE_TT_DIGITS,
    BEE_TOKEN_TYPE_COUNT,
};

struct bee_token {
    struct bee_token *next;
    char *base;
    size_t len;
    size_t col;
    size_t row;
    enum bee_token_type token_type;
};

enum bee_keyword {
    BEE_KW_MODULE,
    BEE_KW_IMPORT,
    BEE_KW_EXPORT,
    BEE_KW_EXTERN,
    BEE_KW_PRIVATE,
    BEE_KW_CONST,
    BEE_KW_VAL,
    BEE_KW_VAR,
    BEE_KW_END,
    BEE_KW_STRUCT,
    BEE_KW_ENUM,
    BEE_KW_PROC,
    BEE_KW_PASS,
    BEE_KW_ASSERT,
    BEE_KW_WHILE,
    BEE_KW_FOR,
    BEE_KW_IN,
    BEE_KW_AS,
    BEE_KW_LABEL,
    BEE_KW_BREAK,
    BEE_KW_CONTINUE,
    BEE_KW_IF,
    BEE_KW_ELIF,
    BEE_KW_ELSE,
    BEE_KW_SWITCH,
    BEE_KW_CASE,
    BEE_KW_FALLTHROUGH,
    BEE_KW_DEFAULT,
    BEE_KW_TYPE,
    BEE_KW_NOT,
    BEE_KW_AND,
    BEE_KW_OR,
    BEE_KW_TRUE,
    BEE_KW_FALSE,
    BEE_KEYWORDS_COUNT,
};

struct bee_token *bee_token_new();
void bee_token_free(struct bee_token *token);

bool is_space(uint32_t codepoint);
bool is_eol(uint32_t codepoint);
bool is_punct(uint32_t codepoint);
bool is_alpha(uint32_t codepoint);
bool is_digit(uint32_t codepoint);
bool is_alphanum(uint32_t codepoint);
bool is_keyword(const char *base, size_t len);

void bee_tokenize_buffer(char *data_buf, size_t data_len,
                         struct bee_token *token_start, struct bee_error *error_start);

#endif //BEE_LEXER_H
