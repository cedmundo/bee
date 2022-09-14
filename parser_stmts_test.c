//
// Created by carlos on 9/13/22.
//
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"

static void test_parse_call_or_assign_statement(void **state) {
    UNUSED(state);

    struct bee_error error = {0};
    struct bee_token start;
    struct bee_token rest;
    struct bee_ast_node *node;
    struct bee_ast_node *aux0;

    bee_error_clear(&error);
    start = bee_token_start("test_parse_call_or_assign_statement", "a()");
    rest = bee_token_next(start, &error);
    node = bee_parse_call_or_assign_stmt(&rest, &error);
    assert_non_null(node);
    assert_false(bee_error_is_set(&error));
    assert_int_equal(node->tag, BEE_AST_NODE_CALL_EXPR);

    aux0 = node->as_pair.left;
    assert_int_equal(aux0->tag, BEE_AST_NODE_ID_EXPR);
    assert_string_equal(aux0->as_str, "a");
    bee_ast_node_free(node);
}

int main() {
    UNUSED_TYPE(jmp_buf);
    UNUSED_TYPE(va_list);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_call_or_assign_statement),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}