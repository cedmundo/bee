//
// Created by carlos on 9/3/22.
//

#include "array.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static bool array_grow(struct bee_array *array);

struct bee_array *bee_array_new(size_t default_cap, size_t growth_size) {
    assert(default_cap > 0 && "bee_array_new: default_cap must be greater than zero");
    assert(growth_size > 0 && "bee_array_new: growth_size must be greater than zero");
    struct bee_array *array = bee_rc_alloc(sizeof(struct bee_array), bee_array_release);
    array->data = calloc(default_cap, sizeof(uintptr_t));
    if (array->data == NULL) {
        bee_rc_dec(array);
        return NULL;
    }

    array->cap = default_cap;
    array->growth_size = growth_size;
    array->head = -1;
    return array;
}

void bee_array_release(void *maybe_array) {
    assert(maybe_array != NULL && "bee_array_release: maybe_array must not be null");
    struct bee_array *array = (struct bee_array *) maybe_array;
    for (size_t i=0;i<array->cap; i++) {
        if (array->data[i] != NULL) {
            bee_rc_dec(array->data[i]);
        }
    }

    free(array->data);
}

enum bee_array_op_res bee_array_get(struct bee_array *array, size_t idx, void **dst) {
    assert(array != NULL && "bee_array_get: array must not be null");
    assert(dst != NULL && "bee_array_get: dst must not be null");
    if (idx >= array->cap) {
        return BEE_OP_RES_INDEX_OUT_OF_RANGE;
    }

    *dst = array->data[idx];
    return BEE_OP_RES_OK;
}

enum bee_array_op_res bee_array_set(struct bee_array *array, size_t idx, void *src) {
    assert(array != NULL && "bee_array_set: array must not be null");
    assert(src != NULL && "bee_array_set: dst must not be null");
    if (idx >= array->cap) {
        return BEE_OP_RES_INDEX_OUT_OF_RANGE;
    }

    void *dst = array->data[idx];
    array->data[idx] = bee_rc_inc(src); // We are now referencing this new object
    if (dst != NULL) {
        bee_rc_dec(dst); // We are no longer referencing this old object
    }

    return BEE_OP_RES_OK;
}

bool bee_array_is_empty(struct bee_array *array) {
    return array->head == -1;
}

size_t bee_array_len(struct bee_array *array) {
    return array->head + 1;
}

size_t bee_array_cap(struct bee_array *array) {
    return array->cap;
}

enum bee_array_op_res bee_array_push_back(struct bee_array *array, void *src) {
    assert(array != NULL && "bee_array_push_back: array must not be null");
    assert(src != NULL && "bee_array_push_back: src must not be null");
    int64_t insert_at = ++array->head;
    if (insert_at >= array->cap) {
        if (!array_grow(array)) {
            return BEE_OP_RES_OUT_OF_MEMORY;
        }
    }

    return bee_array_set(array, insert_at, src);
}


enum bee_array_op_res bee_array_peek_back(struct bee_array *array, void **dst) {
    assert(array != NULL && "bee_array_peek_back: array must not be null");
    assert(dst != NULL && "bee_array_peek_back: dst must not be null");
    if (bee_array_is_empty(array)) {
        return BEE_OP_RES_INDEX_OUT_OF_RANGE;
    }

    return bee_array_get(array, array->head, dst);
}

enum bee_array_op_res bee_array_pop_back(struct bee_array *array, void **dst) {
    assert(array != NULL && "bee_array_pop_back: array must not be null");
    if (dst != NULL) {
        enum bee_array_op_res status = bee_array_peek_back(array, dst);
        bee_rc_inc(*dst);
        if (status != BEE_OP_RES_OK) {
            return status;
        }
    }

    if (array->head > -1) {
        bee_rc_dec(array->data[array->head]);
        array->data[array->head] = NULL;
        array->head--;
    }
    return BEE_OP_RES_OK;
}

static bool array_grow(struct bee_array *array) {
    size_t old_cap = array->cap;
    array->cap += array->growth_size;
    array->data = realloc(array->data, array->cap * sizeof(uintptr_t));
    for (size_t i=old_cap;i<array->cap;i++) {
        array->data[i] = NULL;
    }
    return array->data != NULL;
}
