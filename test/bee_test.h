//
// Created by carlos on 9/26/21.
//

#ifndef BEE_BEE_TEST_H
#define BEE_BEE_TEST_H
#include <gtest/gtest.h>

extern "C" {
#include <bee/bee.h>
};

void assert_is_number_ast_node(bee_ast_node *node, bool with_value, int32_t value);

#endif //BEE_BEE_TEST_H
