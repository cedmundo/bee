//
// Created by carlos on 7/6/22.
//

#ifndef BEE_COMPILER_H
#define BEE_COMPILER_H
#include <jit/jit-value.h>
#include "parser.h"
#include "scope.h"

enum bee_compiler_error_type {
    BEE_COMPILER_ERROR_NONE,
    BEE_COMPILER_ERROR_UNSUPPORTED_NODE,
    BEE_COMPILER_UNDEFINED_SYMBOL,
};

struct bee_compiler_error {
    char msg[500];
    const char *filename;
    size_t row;
    size_t col;
    enum bee_compiler_error_type type;
};

jit_value_t bee_compile_node(jit_function_t f, struct bee_ast_node *node, struct bee_compiler_error *error, struct bee_scope *scope);

#endif //BEE_COMPILER_H
