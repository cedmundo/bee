//
// Created by carlos on 9/3/22.
//

#include "dynamic_array.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static bool dynamic_array_grow(struct bee_dynamic_array *array) {
    array->elem_cap += array->growth_size;
    array->data = realloc(array->data, array->elem_cap);
    return array->data != NULL;
}

struct bee_dynamic_array *bee_dynamic_array_new(size_t elem_size, size_t default_cap, size_t growth_size, destructor_f free_f) {
    assert(elem_size > 0 && "bee_dynamic_array_new: elem_size must be greater than zero");
    assert(default_cap > 0 && "bee_dynamic_array_new: default_cap must be greater than zero");
    assert(growth_size > 0 && "bee_dynamic_array_new: growth_size must be greater than zero");

    struct bee_dynamic_array *array = calloc(1, sizeof(struct bee_dynamic_array));
    array->data = calloc(default_cap, elem_size);
    if (array->data == NULL) {
        free(array);
        return NULL;
    }

    array->elem_cap = default_cap;
    array->growth_size = growth_size;
    array->elem_size = elem_size;
    array->free_f = free_f;
    array->stack_top = -1;
    return array;
}

void bee_dynamic_array_free(void *maybe_array) {
    assert(maybe_array != NULL && "bee_dynamic_array_free: maybe_array must not be null");
    struct bee_dynamic_array *array = (struct bee_dynamic_array *) maybe_array;
    if (array->free_f != NULL) {
        for (size_t i=0;i<array->elem_cap;i++) {
            void *offset = array->data + i * array->elem_size;
            array->free_f(offset);
        }
    }
    free(array->data);
}

enum bee_da_status bee_dynamic_array_get(struct bee_dynamic_array *array, size_t idx, void *dst) {
    assert(array != NULL && "bee_dynamic_array_get: array must not be null");
    assert(dst != NULL && "bee_dynamic_array_get: dst must not be null");
    if (idx >= array->elem_cap) {
        return BEE_DA_STATUS_INDEX_OUT_OF_RANGE;
    }

    void *offset = array->data + (idx * array->elem_size);
    assert(memcpy(dst, offset, array->elem_size) == dst);
    return BEE_DA_STATUS_OK;
}

enum bee_da_status bee_dynamic_array_set(struct bee_dynamic_array *array, size_t idx, void *src) {
    assert(array != NULL && "bee_dynamic_array_set: array must not be null");
    assert(src != NULL && "bee_dynamic_array_set: dst must not be null");
    if (idx >= array->elem_cap) {
        return BEE_DA_STATUS_INDEX_OUT_OF_RANGE;
    }

    void *offset = array->data + (idx * array->elem_size);
    assert(memcpy(offset, src, array->elem_size) == offset);
    return BEE_DA_STATUS_OK;
}

bool bee_dynamic_array_is_empty_stack(struct bee_dynamic_array *array) {
    return array->stack_top == -1;
}

size_t bee_dynamic_array_count(struct bee_dynamic_array *array) {
    return array->stack_top+1;
}

enum bee_da_status bee_dynamic_array_push_back(struct bee_dynamic_array *array, void *src) {
    assert(array != NULL && "bee_dynamic_array_push_back: array must not be null");
    assert(src != NULL && "bee_dynamic_array_push_back: src must not be null");
    int64_t insert_at = ++array->stack_top;
    if (insert_at >= array->elem_cap) {
        if (!dynamic_array_grow(array)) {
            return BEE_DA_STATUS_OUT_OF_MEMORY;
        }
    }

    return bee_dynamic_array_set(array, insert_at, src);
}

enum bee_da_status bee_dynamic_array_peek_back(struct bee_dynamic_array *array, void *dst) {
    assert(array != NULL && "bee_dynamic_array_peek_back: array must not be null");
    assert(dst != NULL && "bee_dynamic_array_peek_back: dst must not be null");
    if (bee_dynamic_array_is_empty_stack(array)) {
        return BEE_DA_STATUS_INDEX_OUT_OF_RANGE;
    }

    return bee_dynamic_array_get(array, array->stack_top, dst);
}

enum bee_da_status bee_dynamic_array_pop_back(struct bee_dynamic_array *array, void *dst) {
    assert(array != NULL && "bee_dynamic_array_pop_back: array must not be null");
    if (dst != NULL) {
        enum bee_da_status status = bee_dynamic_array_peek_back(array, dst);
        if (status != BEE_DA_STATUS_OK) {
            return status;
        }
    }

    if (array->stack_top > -1) {
        void *offset = array->data + (array->stack_top * array->elem_size);
        assert(memset(offset, 0L, array->elem_size) == offset);
        array->stack_top--;
    }
    return BEE_DA_STATUS_OK;
}