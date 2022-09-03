//
// Created by carlos on 9/3/22.
//

#ifndef BEE_ERRORS_H
#define BEE_ERRORS_H
#define BEE_ERROR_MSG_SIZE 500
#include <stdbool.h>
#include "common.h"

struct bee_error {
    char msg[BEE_ERROR_MSG_SIZE];
    struct bee_loc loc;
};

// inits an error so it can be safely evaluated if its set or not
void bee_error_init(struct bee_error *error);

// remove the message and location from an error
void bee_error_clear(struct bee_error *error);

// set an error with a formatted message (standard). Error must not be nil.
void bee_error_set(struct bee_error *error, struct bee_loc loc, const char *msg, ...);

// print an error. Error must not be nil.
void bee_error_print(struct bee_error *error);

// return true if error has been written, false otherwise.
bool bee_error_is_set(struct bee_error *error);

#endif //BEE_ERRORS_H
