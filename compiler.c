//
// Created by carlos on 7/6/22.
//
#include <jit/jit-util.h>
#include <jit/jit-type.h>
#include <jit/jit.h>
#include "compiler.h"
#include "scope.h"

static size_t get_arg_count(struct bee_ast_node *root) {
    struct bee_ast_node *node = root;
    size_t count = 0L;
    while (node != NULL && node->type == BEE_AST_NODE_ARG) {
        count++;
        node = node->right;
    }
    return count;
}

static void flatten_args(jit_function_t f, struct bee_ast_node *root, struct bee_compiler_error *error, struct bee_scope *scope, jit_value_t *values) {
    struct bee_ast_node *node = root;
    size_t i = 0L;
    while (node != NULL && node->type == BEE_AST_NODE_ARG) {
        values[i++] = bee_compile_node(f, node->left, error, scope).as_value;
        if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
            return;
        }

        node = node->right;
    }
}

union bee_object bee_compile_node(jit_function_t f, struct bee_ast_node *node, struct bee_compiler_error *error, struct bee_scope *scope) {
    jit_value_t zero = jit_value_create_nint_constant(f, jit_type_int, 0);
    union bee_object result = {.as_value = zero};
    union bee_object aux0;
    union bee_object aux1;

    switch (node->type) {
        case BEE_AST_NODE_LIT_BOL:
            result.as_value = jit_value_create_nint_constant(f, jit_type_sys_bool, node->as_bol);
            break;
        case BEE_AST_NODE_LIT_U8:
            result.as_value = jit_value_create_nint_constant(f, jit_type_ubyte, node->as_u8);
            break;
        case BEE_AST_NODE_LIT_U16:
            result.as_value = jit_value_create_nint_constant(f, jit_type_ushort, node->as_u16);
            break;
        case BEE_AST_NODE_LIT_U32:
            result.as_value = jit_value_create_nint_constant(f, jit_type_uint, node->as_u32);
            break;
        case BEE_AST_NODE_LIT_U64:
            result.as_value = jit_value_create_long_constant(f, jit_type_ulong, (int32_t)node->as_u64);
            break;
        case BEE_AST_NODE_LIT_I8:
            result.as_value = jit_value_create_nint_constant(f, jit_type_sbyte, node->as_i8);
            break;
        case BEE_AST_NODE_LIT_I16:
            result.as_value = jit_value_create_nint_constant(f, jit_type_short, node->as_i16);
            break;
        case BEE_AST_NODE_LIT_I32:
            result.as_value = jit_value_create_nint_constant(f, jit_type_int, node->as_i32);
            break;
        case BEE_AST_NODE_LIT_I64:
            result.as_value = jit_value_create_long_constant(f, jit_type_long, node->as_i64);
            break;
        case BEE_AST_NODE_LIT_F32:
            result.as_value = jit_value_create_float32_constant(f, jit_type_float32, node->as_f32);
            break;
        case BEE_AST_NODE_LIT_F64:
            result.as_value = jit_value_create_float64_constant(f, jit_type_float64, node->as_f64);
            break;
        case BEE_AST_NODE_UNA_BIT_NEG:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_not(f, aux0.as_value);
            break;
        case BEE_AST_NODE_UNA_ARI_POS:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_mul(f, aux0.as_value,
                                           jit_value_create_nint_constant(f, jit_type_int, +1));
            break;
        case BEE_AST_NODE_UNA_ARI_NEG:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_mul(f, aux0.as_value,
                                           jit_value_create_nint_constant(f, jit_type_int, -1));
            break;
        case BEE_AST_NODE_UNA_LOG_NEG:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_to_not_bool(f, jit_insn_to_bool(f, aux0.as_value));
            break;
        case BEE_AST_NODE_BIN_ADD:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_add(f, aux0.as_value,aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_SUB:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_sub(f, aux0.as_value,aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_MUL:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_mul(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_DIV:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_div(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_REM:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_rem(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_EQ:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_eq(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_NE:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_ne(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_GT:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_gt(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_GE:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_ge(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_LT:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_lt(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_LE:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_le(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_LSH:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_shl(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_RSH:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_shr(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_BIT_AND:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_and(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_BIT_OR:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_or(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_BIT_XOR:
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_insn_xor(f, aux0.as_value, aux1.as_value);
            break;
        case BEE_AST_NODE_BIN_LOG_AND: {
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_value_create(f, jit_type_sys_bool);
            jit_label_t false_label = jit_label_undefined;
            jit_insn_store(f, result.as_value, jit_value_create_nint_constant(f, jit_type_sys_bool, false));
            jit_insn_branch_if_not(f, jit_insn_to_bool(f, aux0.as_value), &false_label);
            jit_insn_store(f, result.as_value, jit_insn_to_bool(f, aux1.as_value));
            jit_insn_label(f, &false_label);
            return result;
        }
        case BEE_AST_NODE_BIN_LOG_OR: {
            aux0 = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            aux1 = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            result.as_value = jit_value_create(f, jit_type_sys_bool);
            jit_label_t true_label = jit_label_undefined;
            jit_insn_store(f, result.as_value, jit_value_create_nint_constant(f, jit_type_sys_bool, true));
            jit_insn_branch_if(f, jit_insn_to_bool(f, aux0.as_value), &true_label);
            jit_insn_store(f, result.as_value, jit_insn_to_bool(f, aux1.as_value));
            jit_insn_label(f, &true_label);
            return result;
        }
        case BEE_AST_NODE_ID: {
            if (!bee_scope_get(scope, node->as_str, &result)) {
                result.as_value = zero;
                if (error == NULL) {
                    error = jit_calloc(1, sizeof(struct bee_compiler_error));
                }

                error->type = BEE_COMPILER_UNDEFINED_SYMBOL;
                error->row = node->row;
                error->col = node->col;
                error->filename = node->filename;
                jit_sprintf(error->msg, "undefined symbol: %s", node->as_str);
                return result;
            }

            return result;
        }
        case BEE_AST_NODE_BIN_DUCK_ASSIGN: {
            struct bee_ast_node *id = node->left;
            result = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            bee_scope_bind(scope, id->as_str, result);
            return result;
        }
        case BEE_AST_NODE_LET_IN_EXPR: {
            bee_scope_push(scope);
            bee_compile_node(f, node->left, error, scope); // ignore result, it's already assigned
            result = bee_compile_node(f, node->right, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            bee_scope_pop(scope);
            return result;
        }
        case BEE_AST_NODE_CALL: {
            union bee_object callee = bee_compile_node(f, node->left, error, scope);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            size_t arg_count = get_arg_count(node->right);
            jit_value_t *arg_values = jit_calloc(arg_count, sizeof(jit_value_t));
            // FIXME: Release this pointer with result object

            flatten_args(f, node->right, error, scope, arg_values);
            if (error != NULL && error->type != BEE_COMPILER_ERROR_NONE) {
                return result;
            }

            if (callee.as_func.is_native) {
                result.as_value = jit_insn_call_native(f, callee.as_func.name, callee.as_func.native_addr,
                                     callee.as_func.signature, arg_values, arg_count, callee.as_func.flags);
            } else {
                result.as_value = jit_insn_call(f, callee.as_func.name, callee.as_func.jit_function,
                              callee.as_func.signature, arg_values, arg_count, callee.as_func.flags);
            }
            break;
        }
        case BEE_AST_NODE_LIT_STR:
        case BEE_AST_NODE_NONE:
        case BEE_AST_NODE_TYPES:
        case BEE_AST_NODE_ARG:
            if (error == NULL) {
                error = jit_calloc(1, sizeof(struct bee_compiler_error));
            }

            error->type = BEE_COMPILER_ERROR_UNSUPPORTED_NODE;
            error->row = node->row;
            error->col = node->col;
            error->filename = node->filename;
            jit_sprintf(error->msg, "unsupported construction: %d", node->type);
    }

    return result;
}

