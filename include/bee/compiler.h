//
// Created by carlos on 10/16/21.
//
#ifndef BEE_COMPILER_H
#define BEE_COMPILER_H
#include <bee/parser.h>
#include <jit/jit-value.h>

jit_value_t bee_compile_expr(jit_function_t f, struct bee_ast_node *node);
#endif //BEE_COMPILER_H
