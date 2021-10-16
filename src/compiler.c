//
// Created by carlos on 10/16/21.
//
#include <bee/compiler.h>
#include <jit/jit-common.h>
#include <jit/jit-insn.h>
#include <jit/jit-type.h>
#include <jit/jit-value.h>

jit_value_t bee_compile_expr(jit_function_t f, struct bee_ast_node *node) {
    jit_value_t value;
    switch (node->type) {
        case BEE_AST_NODE_TYPE_I32:
            value = jit_value_create_nint_constant(f, jit_type_int, node->as_i32);
            break;
        case BEE_AST_NODE_TYPE_ADD:
            value = jit_insn_add(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_SUB:
            value = jit_insn_sub(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_MUL:
            value = jit_insn_mul(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_DIV:
            value = jit_insn_div(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_MOD:
            value = jit_insn_rem(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_AND:
            value = jit_insn_and(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_OR:
            value = jit_insn_or(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_XOR:
            value = jit_insn_xor(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_LSHIFT:
            value = jit_insn_shl(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_BIT_RSHIFT:
            value = jit_insn_shr(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_EQ:
            value = jit_insn_eq(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_NEQ:
            value = jit_insn_ne(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_GT:
            value = jit_insn_gt(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_GTE:
            value = jit_insn_ge(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_LT:
            value = jit_insn_lt(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_LTE:
            value = jit_insn_le(f, bee_compile_expr(f, node->left), bee_compile_expr(f, node->right));
            break;
        case BEE_AST_NODE_TYPE_LOG_AND:
        {
            jit_value_t tmp = jit_value_create(f, jit_type_sys_bool);
            jit_label_t a_label = jit_label_undefined;
            jit_label_t b_label = jit_label_undefined;
            jit_insn_branch_if(f, bee_compile_expr(f, node->left), &a_label);
            jit_insn_store(f, tmp, jit_value_create_nint_constant(f, jit_type_sys_bool, 0));
            jit_insn_branch(f, &b_label);

            jit_insn_label(f, &a_label);
            jit_insn_store(f, tmp, bee_compile_expr(f, node->right));

            jit_insn_label(f, &b_label);
            value = jit_insn_load(f, tmp);
            break;
        }
        case BEE_AST_NODE_TYPE_LOG_OR:
        {
            jit_value_t tmp = jit_value_create(f, jit_type_sys_bool);
            jit_label_t a_label = jit_label_undefined;
            jit_label_t b_label = jit_label_undefined;
            jit_insn_branch_if_not(f, bee_compile_expr(f, node->left), &a_label);
            jit_insn_store(f, tmp, jit_value_create_nint_constant(f, jit_type_sys_bool, 1));
            jit_insn_branch(f, &b_label);

            jit_insn_label(f, &a_label);
            jit_insn_store(f, tmp, bee_compile_expr(f, node->right));

            jit_insn_label(f, &b_label);
            value = jit_insn_load(f, tmp);
            break;
        }
        case BEE_AST_NODE_TYPE_ARI_INV_POS:
            value = jit_insn_mul(f, bee_compile_expr(f, node->left), jit_value_create_nint_constant(f, jit_type_int, +1));
            break;
        case BEE_AST_NODE_TYPE_ARI_INV_NEG:
            value = jit_insn_mul(f, bee_compile_expr(f, node->left), jit_value_create_nint_constant(f, jit_type_int, -1));
            break;
        case BEE_AST_NODE_TYPE_BIT_NEG:
            value = jit_insn_not(f, bee_compile_expr(f, node->left));
            break;
        case BEE_AST_NODE_TYPE_LOG_NEG:
            value = jit_insn_neg(f, bee_compile_expr(f, node->left));
            break;
    }
    return value;
}

