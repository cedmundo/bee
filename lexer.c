//
// Created by carlos on 1/19/22.
//

#include "lexer.h"
#include <jit/jit-util.h>

struct bee_token *bee_token_new() {
    return jit_calloc(1, sizeof(struct bee_token));
}

struct bee_token *bee_token_push(struct bee_token *token, char *base,
        size_t len, size_t col, size_t row, enum bee_token_type token_type) {
    token->base = base;
    token->len = len;
    token->col = col;
    token->row = row;
    token->token_type = token_type;
    if (token_type != BEE_TT_EOF) {
        token->next = bee_token_new();
    }

    return token->next;
}

void bee_token_free(struct bee_token *token) {
    struct bee_token *cur = token;
    struct bee_token *tmp;
    while (cur != NULL) {
        tmp = cur->next;
        jit_free(cur);
        cur = tmp;
    }
}

void bee_tokenize_buffer(char *data_buf, size_t data_len,
                         struct bee_token *token_start, struct bee_error *error_start) {

}