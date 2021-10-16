//
// Created by carlos on 10/16/21.
//

#include "bee_test.h"
#include <gtest/gtest.h>

// NOLINTNEXTLINE
TEST(SCOPE, SCOPE_BIND_NO_CONFLICT_FINE) {
    struct bee_scope *root = bee_scope_new();
    struct bee_scope *ret = bee_scope_bind(root, "_id", nullptr);

    ASSERT_EQ(ret, root);
    ASSERT_NE(ret->head, nullptr);
    EXPECT_STREQ(ret->head->id, "_id");

    bee_scope_free(ret);
}

// NOLINTNEXTLINE
TEST(SCOPE, SCOPE_BIND_WITH_CONFLICT_FINE) {
    struct bee_scope *root = bee_scope_new();
    struct bee_scope *ret0 = bee_scope_bind(root, "_id", nullptr);
    struct bee_scope *ret1 = bee_scope_bind(root, "_id", nullptr);

    ASSERT_EQ(ret0, root);
    ASSERT_NE(ret0->head, nullptr);
    EXPECT_STREQ(ret0->head->id, "_id");

    ASSERT_NE(ret1, ret0);
    ASSERT_NE(ret1, root);
    ASSERT_NE(ret1->head, nullptr);
    EXPECT_STREQ(ret1->head->id, "_id");

    bee_scope_free(ret0);
    bee_scope_free(ret1);
}

// NOLINTNEXTLINE
TEST(SCOPE, SCOPE_RESOLVE_FINE) {
    struct bee_scope *root = bee_scope_new();
    struct bee_scope *ret0 = bee_scope_bind(root, "_id", nullptr);
    ASSERT_EQ(ret0, root);

    struct bee_bind *bind = bee_scope_resolve(ret0, "_id");
    ASSERT_NE(bind, nullptr);
    ASSERT_STREQ(bind->id, "_id");
    bee_scope_free(ret0);
}