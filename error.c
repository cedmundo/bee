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

struct bee_error_list *bee_error_list_new() {
    return jit_calloc(1, sizeof(struct bee_error));
}

void bee_error_list_push(struct bee_error_list *errors, size_t row,
                                 size_t col, char *msg, ...) {
    struct bee_error *error = bee_error_new();
    error->row = row;
    error->col = col;
    error->msg = jit_calloc(BEE_ERROR_BUF_SIZE, sizeof(char));

    va_list vargs;
    va_start(vargs, msg);
    vsprintf(error->msg, msg, vargs);
    va_end(vargs);

    if (errors->head == NULL) {
        errors->head = error;
    }

    if (errors->tail != NULL) {
        errors->tail->next = error;
    }

    errors->tail = error;
}

void bee_error_list_free(struct bee_error_list *errors) {
    struct bee_error *cur = errors->head;
    struct bee_error *tmp;
    while (cur != NULL) {
        tmp = cur->next;
        if (cur->msg != NULL) {
            jit_free(cur->msg);
        }

        jit_free(cur);
        cur = tmp;
    }
    jit_free(errors);
}
