//
// Created by carlos on 9/3/22.
//

#ifndef BEE_COMMON_H
#define BEE_COMMON_H
#include <stddef.h>
#define UNUSED(x) ((void)(x))
#define UNUSED_TYPE(x) ((void)(x *) 0)
typedef void(* destructor_f)(void *);

struct bee_loc {
    const char *filename;
    size_t row;
    size_t col;
};

#endif //BEE_COMMON_H
