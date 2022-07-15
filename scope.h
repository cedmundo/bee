//
// Created by carlos on 7/7/22.
//

#ifndef BEE_SCOPE_H
#define BEE_SCOPE_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <jit/jit-value.h>

struct bee_function_ref {
    bool is_native;
    jit_type_t signature;
    jit_function_t jit_function;
    void *native_addr;
    const char *name;
    int32_t flags;
};

union bee_object {
    jit_value_t as_value;
    struct bee_function_ref as_func;
};

struct bee_scope {
    size_t depth;
    size_t cap;
    size_t len;
    char **ids_array;
    union bee_object *objs_array;
    size_t *depths_array;
};

struct bee_scope *bee_scope_new();
void bee_scope_free(struct bee_scope *scope);
bool bee_scope_bind(struct bee_scope *scope, char *id, union bee_object value);
bool bee_scope_get(struct bee_scope *scope, char *id, union bee_object *value);
void bee_scope_push(struct bee_scope *scope);
void bee_scope_pop(struct bee_scope *scope);

#endif //BEE_SCOPE_H
