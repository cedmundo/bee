//
// Created by carlos on 1/19/22.
//

#ifndef BEE_ERROR_H
#define BEE_ERROR_H

#include <stddef.h>
#include <stdbool.h>

struct bee_error {
    struct bee_error *next;
    char *msg;
    size_t col;
    size_t row;
};

struct bee_error_list {
    struct bee_error *head;
    struct bee_error *tail;
};

#define BEE_ERROR_BUF_SIZE 500
struct bee_error *bee_error_new();
struct bee_error_list *bee_error_list_new();

void bee_error_list_push(struct bee_error_list *error_list, size_t row, size_t col,
        char *msg, ...);
void bee_error_list_free(struct bee_error_list *token);

#endif //BEE_ERROR_H
