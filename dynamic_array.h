//
// Created by carlos on 9/3/22.
//

#ifndef BEE_DYNAMIC_ARRAY_H
#define BEE_DYNAMIC_ARRAY_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "common.h"

enum bee_da_status {
    BEE_DA_STATUS_OK,
    BEE_DA_STATUS_INDEX_OUT_OF_RANGE,
    BEE_DA_STATUS_OUT_OF_MEMORY,
};

struct bee_dynamic_array {
    void *data;
    destructor_f free_f;
    size_t growth_size;
    size_t elem_size;
    size_t elem_cap;
    int64_t stack_top;
};

// creates a new dynamic array with a default size and a growth constant
struct bee_dynamic_array *bee_dynamic_array_new(size_t elem_size, size_t default_cap, size_t growth_size, destructor_f free_f);

// frees a dynamic array from memory (recurse destructors)
void bee_dynamic_array_free(void *);

// get an individual element at index idx
enum bee_da_status bee_dynamic_array_get(struct bee_dynamic_array *array, size_t idx, void *dst);

// set an individual element at index idx
enum bee_da_status bee_dynamic_array_set(struct bee_dynamic_array *array, size_t idx, void *src);

// returns true if dynamic_array is treated as a stack and its empty
bool be_dynamic_array_is_empty_stack(struct bee_dynamic_array *array);

// push a value into array as a stack
enum bee_da_status bee_dynamic_array_push_back(struct bee_dynamic_array *array, void *src);

// peeks a value from array as a stack (don't pop, just fetch)
enum bee_da_status bee_dynamic_array_peek_back(struct bee_dynamic_array *array, void *dst);

// pop a value from array as a stack, dst can be null
enum bee_da_status bee_dynamic_array_pop_back(struct bee_dynamic_array *array, void *dst);

#endif //BEE_DYNAMIC_ARRAY_H
