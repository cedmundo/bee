//
// Created by carlos on 6/10/22.
//

#ifndef BEE_LEXER_H
#define BEE_LEXER_H

#include <stddef.h>
#include "errors.h"

enum bee_token_type {
    BEE_TOKEN_TYPE_NONE,
    BEE_TOKEN_TYPE_EOF,
    BEE_TOKEN_TYPE_EOL,
    BEE_TOKEN_TYPE_PUNCT,
    BEE_TOKEN_TYPE_NUMBER,
    BEE_TOKEN_TYPE_STRING,
    BEE_TOKEN_TYPE_BOOLEAN,
    BEE_TOKEN_TYPE_KEYWORD,
    BEE_TOKEN_TYPE_ID,
};

enum bee_keyword_type {
    BEE_KEYWORD_NONE,
    BEE_KEYWORD_LET,
    BEE_KEYWORD_IN,
    BEE_KEYWORD_AND,
    BEE_KEYWORD_OR,
    BEE_KEYWORD_NOT,
    BEE_KEYWORD_TRUE,
    BEE_KEYWORD_FALSE,
    BEE_KEYWORD_TYPES,
};

enum bee_punct_type {
    BEE_PUNCT_NONE,
    BEE_PUNCT_D_GT,     // >>
    BEE_PUNCT_D_LT,     // <<
    BEE_PUNCT_D_EQ,     // ==
    BEE_PUNCT_NE,       // !=
    BEE_PUNCT_GE,       // >=
    BEE_PUNCT_LE,       // <=
    BEE_PUNCT_WALRUS,   // :=
    BEE_PUNCT_GT,       // >
    BEE_PUNCT_LT,       // <
    BEE_PUNCT_PLUS,     // +
    BEE_PUNCT_MINUS,    // -
    BEE_PUNCT_ASTERISK, // *
    BEE_PUNCT_SLASH,    // /
    BEE_PUNCT_PERCENT,  // %
    BEE_PUNCT_AMPERSAND, // &
    BEE_PUNCT_CARET,    // ^
    BEE_PUNCT_VBAR,     // |
    BEE_PUNCT_TILDE,    // ~
    BEE_PUNCT_LPAR,     // (
    BEE_PUNCT_RPAR,     // )
    BEE_PUNCT_COMMA,    // ,
    BEE_PUNCT_TYPES
};

enum bee_num_base {
    BEE_NUM_BASE_DEC,
    BEE_NUM_BASE_HEX,
    BEE_NUM_BASE_OCT,
    BEE_NUM_BASE_BIN,
};

enum bee_num_type {
    BEE_NUM_TYPE_I8,
    BEE_NUM_TYPE_I16,
    BEE_NUM_TYPE_I32,
    BEE_NUM_TYPE_I64,
    BEE_NUM_TYPE_U8,
    BEE_NUM_TYPE_U16,
    BEE_NUM_TYPE_U32,
    BEE_NUM_TYPE_U64,
    BEE_NUM_TYPE_F32,
    BEE_NUM_TYPE_F64,
};

struct bee_token {
    const char *name;
    const char *data;
    size_t off;
    size_t len;
    size_t row;
    size_t col;
    struct bee_error error;
    enum bee_token_type type;
    enum bee_keyword_type keyword_type;
    enum bee_punct_type punct_type;
    enum bee_num_base num_base;
    enum bee_num_type num_type;
};

struct bee_token bee_token_start(const char *name, const char *data);

struct bee_token bee_token_next(struct bee_token prev);

#endif //BEE_LEXER_H
