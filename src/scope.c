//
// Created by carlos on 10/16/21.
//
#include <bee/scope.h>
#include <jit/jit-util.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

struct bee_scope *bee_scope_new() {
    return jit_calloc(1, sizeof(struct bee_scope));
}

struct bee_bind *bee_bind_new() {
    return jit_calloc(1, sizeof(struct bee_bind));
}

struct bee_scope *bee_scope_fork(struct bee_scope *parent) {
    struct bee_scope *scope = bee_scope_new();
    scope->parent = parent;
    return scope;
}

struct bee_scope *bee_scope_bind(struct bee_scope *scope, const char *id, jit_value_t value) {
    assert(id != NULL);
    struct bee_bind *bind = bee_scope_resolve(scope, id);
    if (bind != NULL) {
        scope = bee_scope_fork(scope);
    }

    bind = bee_bind_new();
    bind->id = jit_strdup(id);
    bind->value = value;
    bind->next = NULL;

    if (scope->head == NULL) {
        scope->head = bind;
    }

    if (scope->tail != NULL) {
        scope->tail->next = bind;
    }

    scope->tail = bind;
    return scope;
}

struct bee_bind *bee_scope_resolve(struct bee_scope *scope, const char *id) {
    assert(id != NULL);
    if (scope == NULL) {
        return NULL;
    }

    for (struct bee_bind *cur = scope->head; cur != NULL; cur = cur->next) {
        if (jit_strcmp(cur->id, id) == 0) {
            return cur;
        }
    }

    return bee_scope_resolve(scope->parent, id);
}

void bee_scope_free(struct bee_scope *scope) {
    struct bee_bind *cur = scope->head;
    struct bee_bind *tmp = NULL;
    while (cur != NULL) {
        tmp = cur->next;
        bee_bind_free(cur);
        cur = tmp;
    }

    jit_free(scope);
}

void bee_bind_free(struct bee_bind *bind) {
    jit_free(bind->id);
    jit_free(bind);
}

