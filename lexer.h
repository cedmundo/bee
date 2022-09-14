//
// Created by carlos on 9/3/22.
//

#ifndef BEE_LEXER_H
#define BEE_LEXER_H

#include "common.h"
#include "errors.h"

enum bee_token_tag {
    BEE_TOKEN_TAG_NONE,
    BEE_TOKEN_TAG_EOF,
    BEE_TOKEN_TAG_EOL,
    BEE_TOKEN_TAG_PUNCT,
    BEE_TOKEN_TAG_NUMBER,
    BEE_TOKEN_TAG_STRING,
    BEE_TOKEN_TAG_BOOLEAN,
    BEE_TOKEN_TAG_KEYWORD,
    BEE_TOKEN_TAG_ID
};

enum bee_keyword_tag {
    BEE_KEYWORD_NONE,
    BEE_KEYWORD_MODULE,
    BEE_KEYWORD_IMPORT,
    BEE_KEYWORD_EXTERN,
    BEE_KEYWORD_FUNC,
    BEE_KEYWORD_TYPE,
    BEE_KEYWORD_LET,
    BEE_KEYWORD_IF,
    BEE_KEYWORD_ELIF,
    BEE_KEYWORD_ELSE,
    BEE_KEYWORD_WHILE,
    BEE_KEYWORD_BREAK,
    BEE_KEYWORD_CONTINUE,
    BEE_KEYWORD_RETURN,
    BEE_KEYWORD_ASSERT,
    BEE_KEYWORD_TRUE,
    BEE_KEYWORD_FALSE,
    BEE_KEYWORD_COUNT,
};

enum bee_punct_tag {
    BEE_PUNCT_NONE,
    BEE_PUNCT_EQ_LSH,   // <<=
    BEE_PUNCT_EQ_RSH,   // >>=
    BEE_PUNCT_D_GT,     // >>
    BEE_PUNCT_D_LT,     // <<
    BEE_PUNCT_D_EQ,     // ==
    BEE_PUNCT_NE,       // !=
    BEE_PUNCT_GE,       // >=
    BEE_PUNCT_LE,       // <=
    BEE_PUNCT_WALRUS,   // :=
    BEE_PUNCT_D_PIPE,   // ||
    BEE_PUNCT_D_AMP,    // &&
    BEE_PUNCT_EQ_ADD,   // +=
    BEE_PUNCT_EQ_SUB,   // +-
    BEE_PUNCT_EQ_AST,   // *=
    BEE_PUNCT_EQ_SLASH, // /=
    BEE_PUNCT_EQ_PCT,   // %=
    BEE_PUNCT_EQ_AMP,   // &=
    BEE_PUNCT_EQ_PIPE,  // |=
    BEE_PUNCT_EQ_CARET, // ^=
    BEE_PUNCT_COLON,    // :
    BEE_PUNCT_GT,       // >
    BEE_PUNCT_LT,       // <
    BEE_PUNCT_PLUS,     // +
    BEE_PUNCT_MINUS,    // -
    BEE_PUNCT_AST,      // *
    BEE_PUNCT_SLASH,    // /
    BEE_PUNCT_PCT,      // %
    BEE_PUNCT_AMP,      // &
    BEE_PUNCT_CARET,    // ^
    BEE_PUNCT_PIPE,     // |
    BEE_PUNCT_TILDE,    // ~
    BEE_PUNCT_LPAR,     // (
    BEE_PUNCT_RPAR,     // )
    BEE_PUNCT_COMMA,    // ,
    BEE_PUNCT_BANG,     // !
    BEE_PUNCT_DOT,      // .
    BEE_PUNCT_LCBR,     // {
    BEE_PUNCT_RCBR,     // }
    BEE_PUNCT_EQ,       // =
    BEE_PUNCT_COUNT
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
    struct bee_loc loc;
    const char *data;
    size_t len;
    enum bee_token_tag tag;
    enum bee_keyword_tag keyword_tag;
    enum bee_punct_tag punct_tag;
    enum bee_num_base num_base;
    enum bee_num_type num_type;
};

// creates a startup token with initial position
struct bee_token bee_token_start(const char *filename, const char *data);

// returns the next token after the given token, sets error if any problem occurs during lexing
struct bee_token bee_token_next(struct bee_token prev, struct bee_error *error);

#endif //BEE_LEXER_H
