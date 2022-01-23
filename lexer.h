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

enum bee_punct {
    BEE_PN_PAR_OPEN,
    BEE_PN_PAR_CLOSE,
    BEE_PN_SBR_OPEN,
    BEE_PN_SBR_CLOSE,
    BEE_PN_CBR_OPEN,
    BEE_PN_CBR_CLOSE,
    BEE_PN_SELF_ADD,
    BEE_PN_SELF_SUB,
    BEE_PN_SELF_DIV,
    BEE_PN_SELF_MUL,
    BEE_PN_SELF_REM,
    BEE_PN_SELF_BIT_LSH,
    BEE_PN_SELF_BIT_RSH,
    BEE_PN_SELF_BIT_AND,
    BEE_PN_SELF_BIT_OR,
    BEE_PN_SELF_BIT_NOT,
    BEE_PN_SEMICOLON,
    BEE_PN_EXPANSION,
    BEE_PN_COLON,
    BEE_PN_DOT,
    BEE_PN_ADD,
    BEE_PN_SUB,
    BEE_PN_DIV,
    BEE_PN_MUL,
    BEE_PN_REM,
    BEE_PN_BIT_LSH,
    BEE_PN_BIT_RSH,
    BEE_PN_LOG_AND,
    BEE_PN_LOG_OR,
    BEE_PN_LOG_NOT,
    BEE_PN_BIT_AND,
    BEE_PN_BIT_OR,
    BEE_PN_BIT_NOT,
    BEE_PN_BIT_XOR,
    BEE_PN_EQ,
    BEE_PN_NE,
    BEE_PN_LE,
    BEE_PN_LT,
    BEE_PN_GE,
    BEE_PN_GT,
    BEE_PN_WALRUS,
    BEE_PN_ASSIGN,
    BEE_PN_TYPE_ARROW,
    BEE_PN_LAMBDA_ARROW,
    BEE_PUNCTS_COUNT,
};

struct bee_token *bee_token_new();
struct bee_token *bee_token_push(struct bee_token *token, char *base,
        size_t len, size_t col, size_t row, enum bee_token_type token_type);
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
