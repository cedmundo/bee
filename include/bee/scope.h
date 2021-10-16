//
// Created by carlos on 10/16/21.
//

#ifndef BEE_SCOPE_H
#define BEE_SCOPE_H
#include <jit/jit-value.h>
#include <stdbool.h>

struct bee_bind {
    struct bee_bind *next;
    char *id;
    jit_value_t value;
};

struct bee_scope {
    struct bee_bind *head;
    struct bee_bind *tail;
    struct bee_scope *parent;
};

// Returns a new empty scope on heap
struct bee_scope *bee_scope_new();

// Returns a new empty bind on heap
struct bee_bind *bee_bind_new();

// Returns a forked scope children of parent
struct bee_scope *bee_scope_fork(struct bee_scope *parent);

// Defines a new value into given scope, if already defined then will fork and define the value within that forked scope
struct bee_scope *bee_scope_bind(struct bee_scope *scope, const char *id, jit_value_t value);

// Will try to resolve the id within innermost scope up to root scope
struct bee_bind *bee_scope_resolve(struct bee_scope *scope, const char *id);

// Release memory created by a scope
void bee_scope_free(struct bee_scope *scope);

// Release memory created by a bind
void bee_bind_free(struct bee_bind *bind);

#endif //BEE_SCOPE_H
