//
// Created by carlos on 9/26/21.
//

#ifndef BEE_BEE_TEST_H
#define BEE_BEE_TEST_H
#include <gtest/gtest.h>

extern "C" {
#include <bee/bee.h>
};

void assert_is_number_ast_node(bee_ast_node *node, bool with_value, int32_t value) {
    ASSERT_NE(nullptr, node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_I32, node->type);
    if (with_value) {
        EXPECT_EQ(value, node->as_i32);
    }
}

#endif //BEE_BEE_TEST_H
