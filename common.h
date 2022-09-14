//
// Created by carlos on 9/3/22.
//

#ifndef BEE_COMMON_H
#define BEE_COMMON_H
#include <stddef.h>
#include <malloc.h>
#include <assert.h>
#define UNUSED(x) ((void)(x))
#define UNUSED_TYPE(x) ((void)(x *) 0)

struct bee_loc {
    const char *filename;
    size_t row;
    size_t col;
};

typedef void(* bee_free_f)(void *);
struct bee_rc_header;

void *bee_rc_alloc(size_t size, bee_free_f free_f);
void *bee_rc_realloc(void *data, size_t new_size);
void *bee_rc_inc(void *handle);
void *bee_rc_dec(void *handle);
size_t bee_rc_get(void *handle);

#endif //BEE_COMMON_H
