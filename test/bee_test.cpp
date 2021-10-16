//
// Created by carlos on 9/26/21.
//
#include "bee_test.h"
#include <gtest/gtest.h>

void assert_is_number_ast_node(bee_ast_node *node, bool with_value, int32_t value) {
    ASSERT_NE(nullptr, node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_I32, node->type);
    if (with_value) {
        EXPECT_EQ(value, node->as_i32);
    }
}