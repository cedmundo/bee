//
// Created by carlos on 7/7/22.
//

#include "scope.h"
#include <jit/jit-util.h>

#define SCOPE_EXPAND_SIZE 120

bool scope_expand(struct bee_scope *scope) {
    size_t new_cap = scope->cap + SCOPE_EXPAND_SIZE;
    if (scope->cap == 0) {
        scope->depths_array = jit_calloc(new_cap, sizeof(size_t));
        if (scope->depths_array == NULL) {
            return false;
        }

        scope->ids_array = jit_calloc(new_cap, sizeof(char*));
        if (scope->ids_array == NULL) {
            jit_free(scope->depths_array);
            return false;
        }

        scope->objs_array = jit_calloc(new_cap, sizeof(jit_value_t));
        if (scope->objs_array == NULL) {
            jit_free(scope->depths_array);
            jit_free(scope->ids_array);
            return false;
        }
    } else if (scope->len >= scope->cap-1) {
        scope->depths_array = jit_realloc(scope->depths_array, new_cap * sizeof(size_t));
        if (scope->depths_array == NULL) {
            return false;
        }

        scope->ids_array = jit_realloc(scope->ids_array, new_cap * sizeof(char*));
        if (scope->ids_array == NULL) {
            jit_free(scope->depths_array);
            return false;
        }

        scope->objs_array = jit_realloc(scope->objs_array, new_cap * sizeof(jit_value_t));
        if (scope->objs_array == NULL) {
            jit_free(scope->depths_array);
            jit_free(scope->ids_array);
            return false;
        }
    }

    scope->cap = scope->cap + SCOPE_EXPAND_SIZE;
    return true;
}

struct bee_scope *bee_scope_new() {
    struct bee_scope *scope = jit_calloc(1, sizeof(struct bee_scope));
    if (scope == NULL) {
        return NULL;
    }

    if (!scope_expand(scope)) {
        bee_scope_free(scope);
        return NULL;
    }

    return scope;
}

void bee_scope_free(struct bee_scope *scope) {
    if (scope->depths_array != NULL) {
        jit_free(scope->depths_array);
    }

    if (scope->ids_array != NULL) {
        jit_free(scope->ids_array);
    }

    if (scope->objs_array != NULL) {
        jit_free(scope->objs_array);
    }

    jit_free(scope);
}

bool bee_scope_bind(struct bee_scope *scope, char *id, union bee_object value) {
    // check if there is a value already defined on the scope
    for (size_t i = scope->len; i > 0; i--) {
        if (scope->depths_array[i] < scope->depth) {
            break;
        }

        if (jit_strcmp(scope->ids_array[i], id) == 0) {
            return false;
        }
    }

    // increase capacity if filled up
    if (scope->len >= scope->cap - 1) {
        if (!scope_expand(scope)) {
            bee_scope_free(scope);
            return false;
        }
    }

    // insert new value
    size_t a = scope->len + 1;
    scope->ids_array[a] = id;
    scope->depths_array[a] = scope->depth;
    scope->objs_array[a] = value;
    scope->len = a;
    return true;
}

bool bee_scope_get(struct bee_scope *scope, char *id, union bee_object *value) {
    for (size_t i = scope->len; i > 0; i--) {
        if (jit_strcmp(scope->ids_array[i], id) == 0) {
            *value = scope->objs_array[i];
            return true;
        }
    }

    return false;
}

void bee_scope_push(struct bee_scope *scope) {
    scope->depth++;
}

void bee_scope_pop(struct bee_scope *scope) {
    size_t old_depth = scope->depth;
    for (size_t i = scope->len; i > 0; i--) {
        if (scope->depths_array[i] < old_depth) {
            break;
        }

        scope->len--;
    }

    scope->depth--;
}