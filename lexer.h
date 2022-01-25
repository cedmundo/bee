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

static_assert(BEE_KEYWORDS_COUNT == 34, "Define keyword string here");
static const char *bee_keywords_table[] = {
        [BEE_KW_MODULE] = "module",
        [BEE_KW_IMPORT] = "import",
        [BEE_KW_EXPORT] = "export",
        [BEE_KW_EXTERN] = "extern",
        [BEE_KW_PRIVATE] = "private",
        [BEE_KW_CONST] = "const",
        [BEE_KW_VAL] = "val",
        [BEE_KW_VAR] = "var",
        [BEE_KW_END] = "end",
        [BEE_KW_STRUCT] = "struct",
        [BEE_KW_ENUM] = "enum",
        [BEE_KW_PROC] = "proc",
        [BEE_KW_PASS] = "pass",
        [BEE_KW_ASSERT] = "assert",
        [BEE_KW_WHILE] = "while",
        [BEE_KW_FOR] = "for",
        [BEE_KW_IN] = "in",
        [BEE_KW_AS] = "as",
        [BEE_KW_LABEL] = "label",
        [BEE_KW_BREAK] = "break",
        [BEE_KW_CONTINUE] = "continue",
        [BEE_KW_IF] = "if",
        [BEE_KW_ELIF] = "elif",
        [BEE_KW_ELSE] = "else",
        [BEE_KW_SWITCH] = "switch",
        [BEE_KW_CASE] = "case",
        [BEE_KW_FALLTHROUGH] = "fallthrough",
        [BEE_KW_DEFAULT] = "default",
        [BEE_KW_TYPE] = "type",
        [BEE_KW_NOT] = "not",
        [BEE_KW_AND] = "and",
        [BEE_KW_OR] = "or",
        [BEE_KW_TRUE] = "true",
        [BEE_KW_FALSE] = "false",
        NULL
};

struct bee_token *bee_token_new();
void bee_token_free(struct bee_token *token);

bool bee_is_space(uint32_t codepoint);
bool bee_is_eol(uint32_t codepoint);
bool bee_is_punct(uint32_t codepoint);
bool bee_is_alpha(uint32_t codepoint);
bool bee_is_digit(uint32_t codepoint);
bool bee_is_alphanum(uint32_t codepoint);
bool bee_is_keyword(const char *base, size_t len);

bool bee_token_match(struct bee_token *token, enum bee_token_type token_type);
bool bee_keyword_match(struct bee_token *token, enum bee_keyword keyword);
bool bee_punct_match(struct bee_token *token, char punct);

void bee_tokenize_buffer(char *data_buf, size_t data_len,
                         struct bee_token *token_start, struct bee_error *error_start);

#endif //BEE_LEXER_H
