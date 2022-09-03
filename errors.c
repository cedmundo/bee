//
// Created by carlos on 9/3/22.
//

#include "errors.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

void bee_error_init(struct bee_error *error) {
    assert(error != NULL);
    assert(memset((char *)error->msg, 0, BEE_ERROR_MSG_SIZE) == error->msg);
}

void bee_error_clear(struct bee_error *error) {
    assert(error != NULL);
    assert(memset((char *)error->msg, 0, BEE_ERROR_MSG_SIZE) == error->msg);
    error->loc = (struct bee_loc) {
        .filename = NULL,
        .row = 1L,
        .col = 0L,
    };
}

void bee_error_set(struct bee_error *error, struct bee_loc loc, const char *msg, ...) {
    assert(error != NULL);
    assert(memset((char *)error->msg, 0, BEE_ERROR_MSG_SIZE) == error->msg);
    error->loc = loc;
    va_list args;
    va_start(args, msg);
    vsprintf(error->msg, msg, args);
    va_end(args);
}

void bee_error_print(struct bee_error *error) {
    assert(error != NULL);
    struct bee_loc loc = error->loc;
    fprintf(stderr, "%s:%ld:%ld: %s\n", loc.filename, loc.row, loc.col, error->msg);
}

bool bee_error_is_set(struct bee_error *error) {
    return error->msg[0] != '\0';
}
