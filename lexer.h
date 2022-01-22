//
// Created by carlos on 1/19/22.
//

#ifndef BEE_LEXER_H
#define BEE_LEXER_H

#include "lexer.h"
#include "error.h"
#include <stddef.h>

enum bee_token_type {
    BEE_TT_EOF,
    BEE_TT_PUNCT,
    BEE_TT_KEYWORD,
    BEE_TT_WORD,
    BEE_TT_LIT_STR,
    BEE_TT_LIT_INT,
    BEE_TT_LIT_DEC,
    BEE_TT_LIT_BOL,
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

struct bee_token *bee_token_new();
struct bee_token *bee_token_push(struct bee_token *token, char *base,
        size_t len, size_t col, size_t row, enum bee_token_type token_type);
void bee_token_free(struct bee_token *token);

void bee_tokenize_buffer(char *data_buf, size_t data_len,
                         struct bee_token *token_start, struct bee_error *error_start);

#endif //BEE_LEXER_H
