//
// Created by carlos on 1/19/22.
//

#ifndef BEE_ERROR_H
#define BEE_ERROR_H

#include <stddef.h>

struct bee_error {
    struct bee_error *next;
    char *msg;
    size_t col;
    size_t row;
};

#define BEE_ERROR_BUF_SIZE 500
struct bee_error *bee_error_new();
struct bee_error *bee_error_push(struct bee_error *error, size_t col,
                                 size_t row, char *msg, ...);
void bee_error_free(struct bee_error *token);

#endif //BEE_ERROR_H
