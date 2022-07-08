//
// Created by carlos on 7/6/22.
//
#include <jit/jit-util.h>
#include <jit/jit-type.h>
#include <jit/jit.h>
#include "compiler.h"
#include "scope.h"

jit_value_t bee_compile_node(jit_function_t f, struct bee_ast_node *node, struct bee_compiler_error *error, struct bee_scope *scope) {
    jit_value_t zero = jit_value_create_nint_constant(f, jit_type_int, 0);
    switch (node->type) {
        case BEE_AST_NODE_LIT_BOL:
            return jit_value_create_nint_constant(f, jit_type_sys_bool, node->as_bol);
        case BEE_AST_NODE_LIT_U8:
            return jit_value_create_nint_constant(f, jit_type_ubyte, node->as_u8);
        case BEE_AST_NODE_LIT_U16:
            return jit_value_create_nint_constant(f, jit_type_ushort, node->as_u16);
        case BEE_AST_NODE_LIT_U32:
            return jit_value_create_nint_constant(f, jit_type_uint, node->as_u32);
        case BEE_AST_NODE_LIT_U64:
            return jit_value_create_long_constant(f, jit_type_ulong, node->as_i64);
        case BEE_AST_NODE_LIT_I8:
            return jit_value_create_nint_constant(f, jit_type_sbyte, node->as_i8);
        case BEE_AST_NODE_LIT_I16:
            return jit_value_create_nint_constant(f, jit_type_short, node->as_i16);
        case BEE_AST_NODE_LIT_I32:
            return jit_value_create_nint_constant(f, jit_type_int, node->as_i32);
        case BEE_AST_NODE_LIT_I64:
            return jit_value_create_long_constant(f, jit_type_long, node->as_i64);
        case BEE_AST_NODE_LIT_F32:
            return jit_value_create_float32_constant(f, jit_type_float32, node->as_f32);
        case BEE_AST_NODE_LIT_F64:
            return jit_value_create_float64_constant(f, jit_type_float64, node->as_f64);
        case BEE_AST_NODE_UNA_BIT_NEG:
            return jit_insn_not(f, bee_compile_node(f, node->left, error, scope));
        case BEE_AST_NODE_UNA_ARI_POS:
            return jit_insn_mul(f, bee_compile_node(f, node->left, error, scope),
                                jit_value_create_nint_constant(f, jit_type_int, +1));
        case BEE_AST_NODE_UNA_ARI_NEG:
            return jit_insn_mul(f, bee_compile_node(f, node->left, error, scope),
                                jit_value_create_nint_constant(f, jit_type_int, -1));
        case BEE_AST_NODE_UNA_LOG_NEG:
            return jit_insn_to_not_bool(f, jit_insn_to_bool(f, bee_compile_node(f, node->left, error, scope)));
        case BEE_AST_NODE_BIN_ADD:
            return jit_insn_add(f, bee_compile_node(f, node->left, error, scope),
                                bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_SUB:
            return jit_insn_sub(f, bee_compile_node(f, node->left, error, scope),
                                bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_MUL:
            return jit_insn_mul(f, bee_compile_node(f, node->left, error, scope),
                                bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_DIV:
            return jit_insn_div(f, bee_compile_node(f, node->left, error, scope),
                                bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_REM:
            return jit_insn_rem(f, bee_compile_node(f, node->left, error, scope),
                                bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_EQ:
            return jit_insn_eq(f, bee_compile_node(f, node->left, error, scope),
                               bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_NE:
            return jit_insn_ne(f, bee_compile_node(f, node->left, error, scope),
                               bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_GT:
            return jit_insn_gt(f, bee_compile_node(f, node->left, error, scope),
                               bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_GE:
            return jit_insn_ge(f, bee_compile_node(f, node->left, error, scope),
                               bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_LT:
            return jit_insn_lt(f, bee_compile_node(f, node->left, error, scope),
                               bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_LE:
            return jit_insn_le(f, bee_compile_node(f, node->left, error, scope),
                               bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_LSH:
            return jit_insn_shl(f, bee_compile_node(f, node->left, error, scope),
                                bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_RSH:
            return jit_insn_shr(f, bee_compile_node(f, node->left, error, scope),
                                bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_BIT_AND:
            return jit_insn_and(f, bee_compile_node(f, node->left, error, scope),
                                bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_BIT_OR:
            return jit_insn_or(f, bee_compile_node(f, node->left, error, scope),
                               bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_BIT_XOR:
            return jit_insn_xor(f, bee_compile_node(f, node->left, error, scope),
                                bee_compile_node(f, node->right, error, scope));
        case BEE_AST_NODE_BIN_LOG_AND: {
            jit_value_t value = jit_value_create(f, jit_type_sys_bool);
            jit_label_t false_label = jit_label_undefined;
            jit_insn_store(f, value, jit_value_create_nint_constant(f, jit_type_sys_bool, false));
            jit_insn_branch_if_not(f, jit_insn_to_bool(f, bee_compile_node(f, node->left, error, scope)), &false_label);
            jit_insn_store(f, value, jit_insn_to_bool(f, bee_compile_node(f, node->right, error, scope)));
            jit_insn_label(f, &false_label);
            return value;
        }
        case BEE_AST_NODE_BIN_LOG_OR: {
            jit_value_t value = jit_value_create(f, jit_type_sys_bool);
            jit_label_t true_label = jit_label_undefined;
            jit_insn_store(f, value, jit_value_create_nint_constant(f, jit_type_sys_bool, true));
            jit_insn_branch_if(f, jit_insn_to_bool(f, bee_compile_node(f, node->left, error, scope)), &true_label);
            jit_insn_store(f, value, jit_insn_to_bool(f, bee_compile_node(f, node->right, error, scope)));
            jit_insn_label(f, &true_label);
            return value;
        }
        case BEE_AST_NODE_ID: {
            jit_value_t value;
            if (!bee_scope_get(scope, node->as_str, &value)) {
                error->type = BEE_COMPILER_UNDEFINED_SYMBOL;
                error->row = node->row;
                error->col = node->col;
                error->filename = node->filename;
                jit_sprintf(error->msg, "undefined symbol: %s", node->as_str);
                return zero;
            }

            return value;
        }
        case BEE_AST_NODE_LIT_STR:
        case BEE_AST_NODE_NONE:
            error->type = BEE_COMPILER_ERROR_UNSUPPORTED_NODE;
            error->row = node->row;
            error->col = node->col;
            error->filename = node->filename;
            jit_sprintf(error->msg, "unsupported construction: %d", node->type);
            return zero;
    }
}

