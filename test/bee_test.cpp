//
// Created by carlos on 9/26/21.
//
#include "bee_test.h"
#include <cstdlib>
#include <gtest/gtest.h>

// NOLINTNEXTLINE
TEST(PARSER, PARSE_SPACE_FINE) {
    const char *starting_with_space = " no_spaces";
    const char *head = starting_with_space;
    EXPECT_TRUE(bee_parse_space(&head));

    const char *starting_with_tab = "\tno_spaces";
    head = starting_with_tab;
    EXPECT_TRUE(bee_parse_space(&head));
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_SPACE_WIHTOUT_HEADING_SPACES) {
    const char *starting_without_space = "x no_spaces";
    const char *head = starting_without_space;
    EXPECT_FALSE(bee_parse_space(&head));

    const char *starting_with_newline = "\n no_spaces";
    head = starting_with_newline;
    EXPECT_FALSE(bee_parse_space(&head));
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_DIGIT_FINE) {
    const char *digits = "0123456789a";
    size_t total_digits = 10;

    const char *head = digits;
    for (size_t i = 0; i < total_digits; i++) {
        const char current_digit = *head;
        EXPECT_EQ(current_digit, digits[i]);
        EXPECT_TRUE(bee_parse_digit(&head));
    }

    const char current_digit = *head;
    EXPECT_EQ(current_digit, digits[10]);
    EXPECT_FALSE(bee_parse_digit(&head));
    EXPECT_EQ(current_digit, digits[10]);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_SPACES_FINE) {
    const char *heading_spaces = "    abc";
    const char *head = heading_spaces;
    size_t expected_spaces = 4;
    size_t actual_spaces = bee_parse_spaces(&head);
    EXPECT_EQ(expected_spaces, actual_spaces);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_SPACES_WITHOUT_SPACES) {
    const char *no_spaces = "abc";
    const char *head = no_spaces;
    size_t expected_spaces = 0;
    size_t actual_spaces = bee_parse_spaces(&head);
    EXPECT_EQ(expected_spaces, actual_spaces);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_NUMBER_LITERAL_FINE) {
    const char *a_number = "42069";
    const char *head = a_number;
    struct bee_ast_node *actual_node = bee_parse_number_lit(&head);

    ASSERT_EQ(BEE_AST_NODE_TYPE_I32, actual_node->type);

    int32_t expected_value = 42069;
    EXPECT_EQ(expected_value, actual_node->as_i32);
    bee_ast_node_free(actual_node);
}

// TODO: This is bad, this should be a parse error
//  because a word cannot be next to a number
//TEST(PARSER, PARSE_NUMBER_LITERAL_NEXT_TO_WORD) {
//    const char *a_number_with_text = "10a";
//    const char *head = a_number_with_text;
//    struct bee_ast_node *actual_node = bee_parse_number_lit(&head);
//    ASSERT_EQ(nullptr, actual_node);
//}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_NUMBER_LITERAL_NEXT_TO_SYMBOL) {
    const char *a_number_with_symbol = "10+";
    const char *head = a_number_with_symbol;
    struct bee_ast_node *actual_node = bee_parse_number_lit(&head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_I32, actual_node->type);
    int32_t expected_value = 10;
    EXPECT_EQ(expected_value, actual_node->as_i32);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_NUMBER_LITERAL_NEXT_TO_PARENTHESIS) {
    const char *a_number_with_parenthesis = "10(";
    const char *head = a_number_with_parenthesis;
    struct bee_ast_node *actual_node = bee_parse_number_lit(&head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_I32, actual_node->type);
    int32_t expected_value = 10;
    EXPECT_EQ(expected_value, actual_node->as_i32);
    bee_ast_node_free(actual_node);

    const char *a_number_with_square_bracket = "10[";
    head = a_number_with_square_bracket;
    actual_node = bee_parse_number_lit(&head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_I32, actual_node->type);
    EXPECT_EQ(expected_value, actual_node->as_i32);
    bee_ast_node_free(actual_node);

    const char *a_number_with_curly_bracket = "10{";
    head = a_number_with_curly_bracket;
    actual_node = bee_parse_number_lit(&head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_I32, actual_node->type);
    EXPECT_EQ(expected_value, actual_node->as_i32);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_NUMBER_LITERAL_NO_NUMBER) {
    const char *not_a_number = "not a number";
    const char *head = not_a_number;
    struct bee_ast_node *actual_node = bee_parse_number_lit(&head);
    ASSERT_EQ(nullptr, actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_PRIMARY_EXPR_FINE) {
    const char *between_parenthesis_expr = "(11)";
    const char *head = between_parenthesis_expr;
    struct bee_ast_node *actual_node = bee_parse_primary_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_I32, actual_node->type);
    int32_t expected_value = 11;
    EXPECT_EQ(expected_value, actual_node->as_i32);
    bee_ast_node_free(actual_node);

    const char *numeric_expr = "11";
    head = numeric_expr;
    actual_node = bee_parse_primary_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_I32, actual_node->type);
    EXPECT_EQ(expected_value, actual_node->as_i32);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_PRIMARY_EXPR_NO_PRIMARY) {
    const char *not_a_primary = "%1";
    const char *head = not_a_primary;

    struct bee_ast_node *actual_node = bee_parse_primary_expr(head, &head);
    ASSERT_EQ(nullptr, actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_TERM_EXPR_FINE) {
    const char *mul_between_two_numbers = "2 * 3";
    const char *head = mul_between_two_numbers;

    struct bee_ast_node *actual_node = bee_parse_term_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_MUL, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 2);
    assert_is_number_ast_node(actual_node->right, true, 3);
    bee_ast_node_free(actual_node);

    const char *div_between_two_numbers = "4 / 5";
    head = div_between_two_numbers;
    actual_node = bee_parse_term_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_DIV, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);
    bee_ast_node_free(actual_node);

    const char *rem_between_two_numbers = "6 % 7";
    head = rem_between_two_numbers;
    actual_node = bee_parse_term_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_MOD, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 6);
    assert_is_number_ast_node(actual_node->right, true, 7);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_TERM_EXPR_CHAIN_FINE) {
    const char *expr = "2 * 3 / 4 % 5";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_term_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_MOD, actual_node->type);

    assert_is_number_ast_node(actual_node->right, true, 5);
    struct bee_ast_node *left_node = actual_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_DIV, left_node->type);

    assert_is_number_ast_node(left_node->right, true, 4);
    left_node = left_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_MUL, left_node->type);

    assert_is_number_ast_node(left_node->left, true, 2);
    assert_is_number_ast_node(left_node->right, true, 3);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_TERM_EXPR_INVERSE_CHAIN_FINE) {
    const char *expr = "2 * (3 / (4 % 5))";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_term_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_MUL, actual_node->type);

    assert_is_number_ast_node(actual_node->left, true, 2);
    struct bee_ast_node *right_node = actual_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_DIV, right_node->type);

    assert_is_number_ast_node(right_node->left, true, 3);
    right_node = right_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_MOD, right_node->type);

    assert_is_number_ast_node(right_node->left, true, 4);
    assert_is_number_ast_node(right_node->right, true, 5);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_FACTOR_EXPR_FINE) {
    const char *add_expr = "3 + 4";
    const char *head = add_expr;

    struct bee_ast_node *actual_node = bee_parse_factor_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_ADD, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 3);
    assert_is_number_ast_node(actual_node->right, true, 4);
    bee_ast_node_free(actual_node);

    const char *sub_expr = "4 - 5";
    head = sub_expr;

    actual_node = bee_parse_factor_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_SUB, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_FACTOR_EXPR_CHAIN_FINE) {
    const char *expr = "5 + 4 - 3";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_factor_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_SUB, actual_node->type);

    assert_is_number_ast_node(actual_node->right, true, 3);
    struct bee_ast_node *left_node = actual_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_ADD, left_node->type);
    assert_is_number_ast_node(left_node->left, true, 5);
    assert_is_number_ast_node(left_node->right, true, 4);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_FACTOR_EXPR_INVERSE_CHAIN_FINE) {
    const char *expr = "5 + (4 - 3)";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_factor_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_ADD, actual_node->type);

    assert_is_number_ast_node(actual_node->left, true, 5);
    struct bee_ast_node *right_node = actual_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_SUB, right_node->type);
    assert_is_number_ast_node(right_node->left, true, 4);
    assert_is_number_ast_node(right_node->right, true, 3);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_BITSHIFT_EXPR_FINE) {
    const char *ls_expr = "3 << 4";
    const char *head = ls_expr;

    struct bee_ast_node *actual_node = bee_parse_bitshift_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_LSHIFT, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 3);
    assert_is_number_ast_node(actual_node->right, true, 4);
    bee_ast_node_free(actual_node);

    const char *rs_expr = "4 >> 5";
    head = rs_expr;

    actual_node = bee_parse_bitshift_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_RSHIFT, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_BITSHIFT_EXPR_CHAIN_FINE) {
    const char *expr = "5 >> 4 << 3";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_bitshift_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_LSHIFT, actual_node->type);

    assert_is_number_ast_node(actual_node->right, true, 3);
    struct bee_ast_node *left_node = actual_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_RSHIFT, left_node->type);
    assert_is_number_ast_node(left_node->left, true, 5);
    assert_is_number_ast_node(left_node->right, true, 4);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_BITSHIFT_EXPR_INVERSE_CHAIN_FINE) {
    const char *expr = "5 >> (4 << 3)";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_bitshift_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_RSHIFT, actual_node->type);

    assert_is_number_ast_node(actual_node->left, true, 5);
    struct bee_ast_node *right_node = actual_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_LSHIFT, right_node->type);
    assert_is_number_ast_node(right_node->left, true, 4);
    assert_is_number_ast_node(right_node->right, true, 3);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_BITWISE_EXPR_FINE) {
    const char *or_expr = "3 | 4";
    const char *head = or_expr;

    struct bee_ast_node *actual_node = bee_parse_bitwise_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_OR, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 3);
    assert_is_number_ast_node(actual_node->right, true, 4);
    bee_ast_node_free(actual_node);

    const char *and_expr = "4 & 5";
    head = and_expr;

    actual_node = bee_parse_bitwise_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_AND, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);
    bee_ast_node_free(actual_node);

    const char *xor_expr = "4 ^ 5";
    head = xor_expr;

    actual_node = bee_parse_bitwise_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_XOR, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_BITWISE_EXPR_CHAIN_FINE) {
    const char *expr = "5 | 4 & 3 ^ 2";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_bitwise_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_XOR, actual_node->type);

    assert_is_number_ast_node(actual_node->right, true, 2);
    struct bee_ast_node *left_node = actual_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_AND, left_node->type);
    assert_is_number_ast_node(left_node->right, true, 3);

    left_node = left_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_OR, left_node->type);
    assert_is_number_ast_node(left_node->right, true, 4);
    assert_is_number_ast_node(left_node->left, true, 5);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_BITWISE_EXPR_INVERSE_CHAIN_FINE) {
    const char *expr = "5 | (4 & (3 ^ 2))";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_bitwise_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_OR, actual_node->type);

    assert_is_number_ast_node(actual_node->left, true, 5);
    struct bee_ast_node *right_node = actual_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_AND, right_node->type);
    assert_is_number_ast_node(right_node->left, true, 4);

    right_node = right_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_BIT_XOR, right_node->type);
    assert_is_number_ast_node(right_node->left, true, 3);
    assert_is_number_ast_node(right_node->right, true, 2);

    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_INEQUALITY_EXPR_FINE) {
    const char *ge_expr = "3 >= 4";
    const char *head = ge_expr;

    struct bee_ast_node *actual_node = bee_parse_inequality_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_GTE, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 3);
    assert_is_number_ast_node(actual_node->right, true, 4);
    bee_ast_node_free(actual_node);

    const char *le_expr = "4 <= 5";
    head = le_expr;

    actual_node = bee_parse_inequality_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LTE, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);
    bee_ast_node_free(actual_node);

    const char *gt_expr = "4 > 5";
    head = gt_expr;

    actual_node = bee_parse_inequality_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_GT, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);
    bee_ast_node_free(actual_node);

    const char *lt_expr = "4 < 5";
    head = lt_expr;

    actual_node = bee_parse_inequality_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LT, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);

    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_INEQUALITY_EXPR_CHAIN_FINE) {
    const char *expr = "5 >= 4 <= 3 > 2 < 1";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_inequality_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LT, actual_node->type);

    assert_is_number_ast_node(actual_node->right, true, 1);
    struct bee_ast_node *left_node = actual_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_GT, left_node->type);
    assert_is_number_ast_node(left_node->right, true, 2);

    left_node = left_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LTE, left_node->type);
    assert_is_number_ast_node(left_node->right, true, 3);

    left_node = left_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_GTE, left_node->type);
    assert_is_number_ast_node(left_node->right, true, 4);
    assert_is_number_ast_node(left_node->left, true, 5);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_INEQUALITY_EXPR_INVERSE_CHAIN_FINE) {
    const char *expr = "5 >= (4 <= (3 > (2 < 1)))";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_inequality_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_GTE, actual_node->type);

    assert_is_number_ast_node(actual_node->left, true, 5);
    struct bee_ast_node *right_node = actual_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LTE, right_node->type);
    assert_is_number_ast_node(right_node->left, true, 4);

    right_node = right_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_GT, right_node->type);
    assert_is_number_ast_node(right_node->left, true, 3);

    right_node = right_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LT, right_node->type);
    assert_is_number_ast_node(right_node->left, true, 2);
    assert_is_number_ast_node(right_node->right, true, 1);

    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_COMPARISON_EXPR_FINE) {
    const char *neq_expr = "3 != 4";
    const char *head = neq_expr;

    struct bee_ast_node *actual_node = bee_parse_comparison_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_NEQ, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 3);
    assert_is_number_ast_node(actual_node->right, true, 4);
    bee_ast_node_free(actual_node);

    const char *eq_expr = "4 == 5";
    head = eq_expr;

    actual_node = bee_parse_comparison_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_EQ, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_COMPARISON_EXPR_CHAIN_FINE) {
    const char *expr = "5 != 4 == 3";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_comparison_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_EQ, actual_node->type);

    assert_is_number_ast_node(actual_node->right, true, 3);
    struct bee_ast_node *left_node = actual_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_NEQ, left_node->type);
    assert_is_number_ast_node(left_node->right, true, 4);
    assert_is_number_ast_node(left_node->left, true, 5);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_COMPARISON_EXPR_INVERSE_CHAIN_FINE) {
    const char *expr = "5 != (4 == 3)";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_comparison_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_NEQ, actual_node->type);

    assert_is_number_ast_node(actual_node->left, true, 5);
    struct bee_ast_node *right_node = actual_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_EQ, right_node->type);
    assert_is_number_ast_node(right_node->left, true, 4);
    assert_is_number_ast_node(right_node->right, true, 3);

    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_LOGICAL_EXPR_FINE) {
    const char *or_expr = "3 || 4";
    const char *head = or_expr;

    struct bee_ast_node *actual_node = bee_parse_logical_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LOG_OR, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 3);
    assert_is_number_ast_node(actual_node->right, true, 4);
    bee_ast_node_free(actual_node);

    const char *and_expr = "4 && 5";
    head = and_expr;

    actual_node = bee_parse_logical_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LOG_AND, actual_node->type);
    assert_is_number_ast_node(actual_node->left, true, 4);
    assert_is_number_ast_node(actual_node->right, true, 5);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_LOGICAL_EXPR_CHAIN_FINE) {
    const char *expr = "5 || 4 && 3";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_logical_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LOG_AND, actual_node->type);

    assert_is_number_ast_node(actual_node->right, true, 3);
    struct bee_ast_node *left_node = actual_node->left;
    ASSERT_NE(nullptr, left_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LOG_OR, left_node->type);
    assert_is_number_ast_node(left_node->left, true, 5);
    assert_is_number_ast_node(left_node->right, true, 4);
    bee_ast_node_free(actual_node);
}

// NOLINTNEXTLINE
TEST(PARSER, PARSE_LOGICAL_EXPR_INVERSE_CHAIN_FINE) {
    const char *expr = "5 || (4 && 3)";
    const char *head = expr;

    struct bee_ast_node *actual_node = bee_parse_logical_expr(head, &head);
    ASSERT_NE(nullptr, actual_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LOG_OR, actual_node->type);

    assert_is_number_ast_node(actual_node->left, true, 5);
    struct bee_ast_node *right_node = actual_node->right;
    ASSERT_NE(nullptr, right_node);
    ASSERT_EQ(BEE_AST_NODE_TYPE_LOG_AND, right_node->type);
    assert_is_number_ast_node(right_node->left, true, 4);
    assert_is_number_ast_node(right_node->right, true, 3);
    bee_ast_node_free(actual_node);
}
