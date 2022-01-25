//
// Created by carlos on 1/19/22.
//

#include "error.h"
#include <stdio.h>
#include <stdarg.h>
#include <jit/jit-util.h>

struct bee_error *bee_error_new() {
    return jit_calloc(1, sizeof(struct bee_error));
}

struct bee_error *bee_error_push(struct bee_error *error, size_t row,
                                 size_t col, char *msg, ...) {
    error->row = row;
    error->col = col;
    error->msg = jit_calloc(BEE_ERROR_BUF_SIZE, sizeof(char));
    error->next = bee_error_new();

    va_list vargs;
    va_start(vargs, msg);
    vsprintf(error->msg, msg, vargs);
    va_end(vargs);
    return error->next;
}

void bee_error_free(struct bee_error *token) {
    struct bee_error *cur = token;
    struct bee_error *tmp;
    while (cur != NULL) {
        tmp = cur->next;
        if (cur->msg != NULL) {
            jit_free(cur->msg);
        }

        jit_free(cur);
        cur = tmp;
    }
}
