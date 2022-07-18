//
// Created by carlos on 7/17/22.
//

#include "errors.h"
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <jit/jit-util.h>

void bee_set_no_error(struct bee_error *error, const char *filename) {
    assert(error != NULL && "error is null");
    error->type = BEE_ERROR_NONE;
    error->filename = filename;
    error->row = 0L;
    error->col = 0L;
    jit_strcpy(error->msg, "no error");
}

void bee_set_error(struct bee_error *error, enum bee_error_type type,
                   const char *filename, size_t row, size_t col, const char *fmt, ...) {
    assert(error != NULL && "error is null");
    error->type = type;
    error->filename = filename;
    error->row = row;
    error->col = col;

    if (fmt != NULL) {
        va_list(args);
        va_start(args, fmt);
        vsprintf(error->msg, fmt, args);
        va_end(args);
    }
}

void bee_set_error_type(struct bee_error *error, enum bee_error_type type, const char *fmt, ...) {
    assert(error != NULL && "error is null");
    error->type = type;
    if (fmt != NULL) {
        va_list(args);
        va_start(args, fmt);
        vsprintf(error->msg, fmt, args);
        va_end(args);
    }
}

bool bee_has_error(struct bee_error *error) {
    assert(error != NULL && "error is null");
    return error->type != BEE_ERROR_NONE;
}

void bee_print_error(struct bee_error *error) {
    assert(error != NULL && "error is null");
    fprintf(stderr, "%s:%ld:%ld: %s\n", error->filename, error->row, error->col, error->msg);
}