//
// Created by carlos on 9/3/22.
//

#ifndef BEE_ARRAY_H
#define BEE_ARRAY_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "common.h"

enum bee_array_op_res {
    BEE_OP_RES_OK,
    BEE_OP_RES_INDEX_OUT_OF_RANGE,
    BEE_OP_RES_OUT_OF_MEMORY,
};

struct bee_array {
    void **data;
    size_t growth_size;
    size_t cap;
    int64_t head;
};

#define into_cell(c) ((void **)&(c))

// creates a new dynamic array with a default size and a growth constant
struct bee_array *bee_array_new(size_t default_cap, size_t growth_size);

// frees a dynamic array from memory (recurse destructors)
void bee_array_release(void *);

// get an individual element at index idx
enum bee_array_op_res bee_array_get(struct bee_array *array, size_t idx, void **dst);

// set an individual element value at index idx, incs refs for src and dec refs for cell
enum bee_array_op_res bee_array_set(struct bee_array *array, size_t idx, void *src);

// returns true if dynamic_array is treated as a stack and its empty
bool bee_array_is_empty(struct bee_array *array);

// returns the count of elements on a stack
size_t bee_array_len(struct bee_array *array);

// returns the total of elements that this array can hold
size_t bee_array_cap(struct bee_array *array);

// push a value into array as a stack
enum bee_array_op_res bee_array_push_back(struct bee_array *array, void *src);

// peeks a value from array as a stack (don't pop, just fetch)
enum bee_array_op_res bee_array_peek_back(struct bee_array *array, void **dst);

// pop a value from array as a stack, dst can be null
enum bee_array_op_res bee_array_pop_back(struct bee_array *array, void **dst);

#endif //BEE_ARRAY_H
