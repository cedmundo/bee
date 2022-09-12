//
// Created by carlos on 9/3/22.
//

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <memory.h>
#include "parser.h"

struct bee_ast_node *bee_ast_node_new() {
    return calloc(1, sizeof(struct bee_ast_node));
}

static struct bee_ast_node *bee_ast_node_new_unary(struct bee_token at_token,
                                            struct bee_ast_node *left) {
    assert(left != NULL && "bee_ast_node_new_unary: left cannot be null");
    struct bee_ast_node *node = bee_ast_node_new();
    if (at_token.punct_tag == BEE_PUNCT_PLUS) {
        node->tag = BEE_AST_NODE_UNA_ARI_POS_EXPR;
    } else if (at_token.punct_tag == BEE_PUNCT_MINUS) {
        node->tag = BEE_AST_NODE_UNA_ARI_NEG_EXPR;
    } else if (at_token.punct_tag == BEE_PUNCT_BANG) {
        node->tag = BEE_AST_NODE_UNA_LOG_NEG_EXPR;
    } else if (at_token.punct_tag == BEE_PUNCT_TILDE) {
        node->tag = BEE_AST_NODE_UNA_BIT_NEG_EXPR;
    } else {
        assert(false && "bee_ast_node_new_unary: unreachable code");
    }

    node->loc = at_token.loc;
    node->as_pair.left = left;
    return node;
}

static struct bee_ast_node *bee_ast_node_new_binary(struct bee_token at_token,
                                             enum bee_ast_node_tag tag,
                                             struct bee_ast_node *left,
                                             struct bee_ast_node *right) {
    assert(tag >= BEE_AST_NODE_BIN_ADD_EXPR && tag <= BEE_AST_NODE_BIN_LOG_OR_EXPR && "bee_ast_node_new_binary: tag is not a binary operator");
    assert(left != NULL && "bee_ast_node_new_unary: left cannot be null");
    assert(right != NULL && "bee_ast_node_new_unary: right cannot be null");
    struct bee_ast_node *node = bee_ast_node_new();
    node->tag = tag;
    node->loc = at_token.loc;
    node->as_pair.left = left;
    node->as_pair.right = right;
    return node;
}

static struct bee_ast_node *bee_ast_node_new_id(struct bee_token token) {
    struct bee_ast_node *node = bee_ast_node_new();
    node->tag = BEE_AST_NODE_ID_EXPR;
    node->loc = token.loc;
    node->as_str = calloc(token.len + 1, sizeof(char));
    memcpy(node->as_str, token.data, token.len);
    return node;
}

static struct bee_ast_node *bee_ast_node_new_literal(struct bee_token token) {
    struct bee_ast_node *node = bee_ast_node_new();
    node->loc = token.loc;
    return node;
}

#define TRACKED_BEE_AST_NODE_TAGS 38
static_assert(TRACKED_BEE_AST_NODE_TAGS == BEE_AST_NODE_COUNT, "Exhaustive ast node name");
const char *bee_ast_node_tag_get_name(enum bee_ast_node_tag tag) {
    static const char *names[] = {
            [BEE_AST_NODE_NONE] = "(none)",
            [BEE_AST_NODE_ID_EXPR] = "id expr",
            [BEE_AST_NODE_PATH_EXPR] = "path expr",
            [BEE_AST_NODE_CALL_EXPR] = "call expr",
            [BEE_AST_NODE_LIT_BOL_EXPR] = "bool expr",
            [BEE_AST_NODE_LIT_I8_EXPR] = "i8 lit expr",
            [BEE_AST_NODE_LIT_I16_EXPR] = "i16 lit expr",
            [BEE_AST_NODE_LIT_I32_EXPR] = "i32 lit expr",
            [BEE_AST_NODE_LIT_I64_EXPR] = "i64 lit expr",
            [BEE_AST_NODE_LIT_U8_EXPR] = "u8 lit expr",
            [BEE_AST_NODE_LIT_U16_EXPR] = "u16 lit expr",
            [BEE_AST_NODE_LIT_U32_EXPR] = "u32 lit expr",
            [BEE_AST_NODE_LIT_U64_EXPR] = "u64 lit expr",
            [BEE_AST_NODE_LIT_F32_EXPR] = "f32 lit expr",
            [BEE_AST_NODE_LIT_F64_EXPR] = "f64 lit expr",
            [BEE_AST_NODE_LIT_STR_EXPR] = "str expr",
            [BEE_AST_NODE_UNA_LOG_NEG_EXPR] = "logical negation expr",
            [BEE_AST_NODE_UNA_BIT_NEG_EXPR] = "bitwise negation expr",
            [BEE_AST_NODE_UNA_ARI_NEG_EXPR] = "arithmetic negation expr",
            [BEE_AST_NODE_UNA_ARI_POS_EXPR] = "arithmetic positive expr",
            [BEE_AST_NODE_BIN_ADD_EXPR] = "add expr",
            [BEE_AST_NODE_BIN_SUB_EXPR] = "sub expr",
            [BEE_AST_NODE_BIN_MUL_EXPR] = "mul expr",
            [BEE_AST_NODE_BIN_DIV_EXPR] = "div expr",
            [BEE_AST_NODE_BIN_REM_EXPR] = "rem expr",
            [BEE_AST_NODE_BIN_EQ_EXPR] = "equal expr",
            [BEE_AST_NODE_BIN_NE_EXPR] = "not equal expr",
            [BEE_AST_NODE_BIN_GT_EXPR] = "greater than expr",
            [BEE_AST_NODE_BIN_GE_EXPR] = "greater or equal than expr",
            [BEE_AST_NODE_BIN_LT_EXPR] = "less than expr",
            [BEE_AST_NODE_BIN_LE_EXPR] = "less or equal than expr",
            [BEE_AST_NODE_BIN_LSH_EXPR] = "left shift expr",
            [BEE_AST_NODE_BIN_RSH_EXPR] = "right shift expr",
            [BEE_AST_NODE_BIN_BIT_AND_EXPR] = "bitwise and expr",
            [BEE_AST_NODE_BIN_BIT_OR_EXPR] = "bitwise or expr",
            [BEE_AST_NODE_BIN_BIT_XOR_EXPR] = "bitwise xor expr",
            [BEE_AST_NODE_BIN_LOG_AND_EXPR] = "logical and expr",
            [BEE_AST_NODE_BIN_LOG_OR_EXPR] = "logical or expr",
            [BEE_AST_NODE_COUNT] = "(invalid)"
    };
    return names[tag];
}

// NOLINTNEXTLINE(misc-no-recursion)
void bee_ast_node_free(void *maybe_node) {
    assert(maybe_node != NULL && "bee_ast_node_free: maybe_node must not be null");
    struct bee_ast_node *node = (struct bee_ast_node *) maybe_node;
    switch (node->tag) {
        case BEE_AST_NODE_LIT_STR_EXPR:
        case BEE_AST_NODE_ID_EXPR:
            if (node->as_str != NULL) {
                free(node->as_str);
            }
            break;
        case BEE_AST_NODE_UNA_BIT_NEG_EXPR:
        case BEE_AST_NODE_UNA_ARI_POS_EXPR:
        case BEE_AST_NODE_UNA_ARI_NEG_EXPR:
        case BEE_AST_NODE_UNA_LOG_NEG_EXPR:
            if (node->as_pair.left != NULL) {
                bee_ast_node_free(node->as_pair.left);
            }
            break;
        case BEE_AST_NODE_BIN_ADD_EXPR:
        case BEE_AST_NODE_BIN_SUB_EXPR:
        case BEE_AST_NODE_BIN_MUL_EXPR:
        case BEE_AST_NODE_BIN_DIV_EXPR:
        case BEE_AST_NODE_BIN_REM_EXPR:
        case BEE_AST_NODE_BIN_EQ_EXPR:
        case BEE_AST_NODE_BIN_NE_EXPR:
        case BEE_AST_NODE_BIN_GT_EXPR:
        case BEE_AST_NODE_BIN_GE_EXPR:
        case BEE_AST_NODE_BIN_LT_EXPR:
        case BEE_AST_NODE_BIN_LE_EXPR:
        case BEE_AST_NODE_BIN_LSH_EXPR:
        case BEE_AST_NODE_BIN_RSH_EXPR:
        case BEE_AST_NODE_BIN_BIT_AND_EXPR:
        case BEE_AST_NODE_BIN_BIT_OR_EXPR:
        case BEE_AST_NODE_BIN_BIT_XOR_EXPR:
        case BEE_AST_NODE_BIN_LOG_AND_EXPR:
        case BEE_AST_NODE_BIN_LOG_OR_EXPR:
            if (node->as_pair.left != NULL) {
                bee_ast_node_free(node->as_pair.left);
            }

            if (node->as_pair.right != NULL) {
                bee_ast_node_free(node->as_pair.right);
            }
            break;
        case BEE_AST_NODE_LIT_BOL_EXPR:
        case BEE_AST_NODE_LIT_I8_EXPR:
        case BEE_AST_NODE_LIT_I16_EXPR:
        case BEE_AST_NODE_LIT_I32_EXPR:
        case BEE_AST_NODE_LIT_I64_EXPR:
        case BEE_AST_NODE_LIT_U8_EXPR:
        case BEE_AST_NODE_LIT_U16_EXPR:
        case BEE_AST_NODE_LIT_U32_EXPR:
        case BEE_AST_NODE_LIT_U64_EXPR:
        case BEE_AST_NODE_LIT_F32_EXPR:
        case BEE_AST_NODE_LIT_F64_EXPR:
            break;
        case BEE_AST_NODE_NONE:
        case BEE_AST_NODE_COUNT:
            assert(false && "unreachable code: invalid tag for node (none or count)");
    }

    free(node);
}

struct bee_ast_node *bee_ast_node_parse_compilation_unit(struct bee_token start_token, struct bee_error *error) {
    struct bee_token rest = bee_token_next(start_token, error);
    if (bee_error_is_set(error)) {
        return NULL;
    }

    return bee_parse_expr(&rest, error);
}

static bool match_tag(struct bee_token token, enum bee_token_tag expected_tag) {
    return token.tag == expected_tag;
}

static bool match_punct(struct bee_token token,
                        enum bee_punct_tag expected_tag) {
    return match_tag(token, BEE_TOKEN_TAG_PUNCT) &&
           token.punct_tag == expected_tag;
}

static bool match_keyword(struct bee_token token,
                          enum bee_keyword_tag expected_tag) {
    return match_tag(token, BEE_TOKEN_TAG_KEYWORD) &&
           token.keyword_tag == expected_tag;
}

static struct bee_token consume(struct bee_token *rest, struct bee_error *error) {
    struct bee_token current = *rest;
    *rest = bee_token_next(current, error);
    return current;
}

// NOLINTNEXTLINE(misc-no-recursion)
struct bee_ast_node *bee_parse_expr(struct bee_token *rest, struct bee_error *error) {
    return bee_parse_binary_expr(rest, error, 0);
}

// NOLINTNEXTLINE(misc-no-recursion)
struct bee_ast_node *bee_parse_binary_expr(struct bee_token *rest, struct bee_error *error, uint32_t prec) {
    enum bee_punct_tag prec_punct_table[][6] = {
            {BEE_PUNCT_D_PIPE,    BEE_PUNCT_NONE,  BEE_PUNCT_NONE,    BEE_PUNCT_NONE, BEE_PUNCT_NONE, BEE_PUNCT_NONE},
            {BEE_PUNCT_D_AMP,     BEE_PUNCT_NONE,  BEE_PUNCT_NONE,    BEE_PUNCT_NONE, BEE_PUNCT_NONE, BEE_PUNCT_NONE},
            {BEE_PUNCT_D_EQ,      BEE_PUNCT_NE,    BEE_PUNCT_GT,      BEE_PUNCT_GE,   BEE_PUNCT_LT,   BEE_PUNCT_LE},
            {BEE_PUNCT_PIPE,      BEE_PUNCT_NONE,  BEE_PUNCT_NONE,    BEE_PUNCT_NONE, BEE_PUNCT_NONE, BEE_PUNCT_NONE},
            {BEE_PUNCT_CARET,     BEE_PUNCT_NONE,  BEE_PUNCT_NONE,    BEE_PUNCT_NONE, BEE_PUNCT_NONE, BEE_PUNCT_NONE},
            {BEE_PUNCT_AMPERSAND, BEE_PUNCT_NONE,  BEE_PUNCT_NONE,    BEE_PUNCT_NONE, BEE_PUNCT_NONE, BEE_PUNCT_NONE},
            {BEE_PUNCT_D_LT,      BEE_PUNCT_D_GT,  BEE_PUNCT_NONE,    BEE_PUNCT_NONE, BEE_PUNCT_NONE, BEE_PUNCT_NONE},
            {BEE_PUNCT_PLUS,      BEE_PUNCT_MINUS, BEE_PUNCT_NONE,    BEE_PUNCT_NONE, BEE_PUNCT_NONE, BEE_PUNCT_NONE},
            {BEE_PUNCT_ASTERISK,  BEE_PUNCT_SLASH, BEE_PUNCT_PERCENT, BEE_PUNCT_NONE, BEE_PUNCT_NONE, BEE_PUNCT_NONE},
    };
    enum bee_ast_node_tag equiv_tag_table[][6] = {
            { BEE_AST_NODE_BIN_LOG_OR_EXPR, BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE},
            { BEE_AST_NODE_BIN_LOG_AND_EXPR, BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE},
            { BEE_AST_NODE_BIN_EQ_EXPR, BEE_AST_NODE_BIN_NE_EXPR,BEE_AST_NODE_BIN_GT_EXPR,BEE_AST_NODE_BIN_GE_EXPR,BEE_AST_NODE_BIN_LT_EXPR,BEE_AST_NODE_BIN_LE_EXPR},
            { BEE_AST_NODE_BIN_BIT_OR_EXPR, BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE},
            { BEE_AST_NODE_BIN_BIT_XOR_EXPR, BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE},
            { BEE_AST_NODE_BIN_BIT_AND_EXPR, BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE},
            { BEE_AST_NODE_BIN_LSH_EXPR, BEE_AST_NODE_BIN_RSH_EXPR,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE},
            { BEE_AST_NODE_BIN_ADD_EXPR, BEE_AST_NODE_BIN_SUB_EXPR,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE},
            { BEE_AST_NODE_BIN_MUL_EXPR, BEE_AST_NODE_BIN_DIV_EXPR,BEE_AST_NODE_BIN_REM_EXPR,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE,BEE_AST_NODE_NONE},
    };

    struct bee_ast_node *node = NULL;
    if (prec >= 8) {
        node = bee_parse_unary_expr(rest, error);
    } else {
        node = bee_parse_binary_expr(rest, error, prec + 1);
    }

    if (bee_error_is_set(error)) {
        bee_ast_node_free(node);
        return NULL;
    }

    bool parse_same_prec = false;
    do {
        for (int8_t i = 0, punct = prec_punct_table[prec][i]; i < 6 && punct != BEE_PUNCT_NONE; i++, punct = prec_punct_table[prec][i]) {
            parse_same_prec = false;

            if (match_punct(*rest, punct)) {
                struct bee_token token = consume(rest, error);
                struct bee_ast_node *right = NULL;
                if (prec >= 8) {
                    right = bee_parse_unary_expr(rest, error);
                } else {
                    right = bee_parse_binary_expr(rest, error, prec + 1);
                }

                node = bee_ast_node_new_binary(token, equiv_tag_table[prec][i], node, right);
                if (bee_error_is_set(error)) {
                    bee_ast_node_free(node);
                    return NULL;
                }

                parse_same_prec = true;
                break;
            }
        }
    } while (parse_same_prec);
    return node;
}

// NOLINTNEXTLINE(misc-no-recursion)
struct bee_ast_node *bee_parse_unary_expr(struct bee_token *rest, struct bee_error *error) {
    if (match_punct(*rest, BEE_PUNCT_PLUS)
        || match_punct(*rest, BEE_PUNCT_MINUS)
        || match_punct(*rest, BEE_PUNCT_BANG)
        || match_punct(*rest, BEE_PUNCT_TILDE)) {
        struct bee_token operator = consume(rest, error);
        return bee_ast_node_new_unary(operator, bee_parse_unary_expr(rest, error));
    }

    return bee_parse_call_expr(rest, error);
}

// NOLINTNEXTLINE(misc-no-recursion)
struct bee_ast_node *bee_parse_call_expr(struct bee_token *rest, struct bee_error *error) {
    struct bee_ast_node *node = bee_parse_primary_expr(rest, error);
    if (match_punct(*rest, BEE_PUNCT_LPAR)) {
        struct bee_token call_start = consume(rest, error);
        struct bee_ast_node *list = bee_ast_node_new();
        list->tag = BEE_AST_NODE_CALL_EXPR;
        list->as_array = bee_dynamic_array_new(sizeof(struct bee_ast_node *), 2, 5, bee_ast_node_free);

        // First element on list it's callee
        if (bee_dynamic_array_push_back(list->as_array, &node) != BEE_DA_STATUS_OK) {
            bee_error_set(error, rest->loc, "could not append an id into a call expr");
            bee_ast_node_free(node);
            return NULL;
        }

        // Next elements on list are arguments
        do {
            if (match_punct(*rest, BEE_PUNCT_RPAR)) {
                break;
            }

            struct bee_ast_node *arg = bee_parse_expr(rest, error);
            if (bee_error_is_set(error)) {
                bee_ast_node_free(node);
                return NULL;
            }

            if (bee_dynamic_array_push_back(list->as_array, &arg) != BEE_DA_STATUS_OK) {
                bee_error_set(error, rest->loc, "could not append an argument into call expr");
                bee_ast_node_free(node);
                return NULL;
            }

            if (match_punct(*rest, BEE_PUNCT_COMMA)) {
                consume(rest, error);
            } else {
                break;
            }
        } while (!match_tag(*rest, BEE_TOKEN_TAG_EOF));

        if (!match_punct(*rest, BEE_PUNCT_RPAR)) {
            bee_error_set(error, rest->loc, "unclosed parenthesis, opening pair was at %ld:%ld", call_start.loc.row, call_start.loc.col);
            bee_ast_node_free(node);
            return NULL;
        }

        consume(rest, error);
        node = list;
    }

    return node;
}

// NOLINTNEXTLINE(misc-no-recursion)
struct bee_ast_node *bee_parse_primary_expr(struct bee_token *rest, struct bee_error *error) {
    if (match_punct(*rest, BEE_PUNCT_LPAR)) {
        struct bee_token par_start = consume(rest, error);
        struct bee_ast_node *node = bee_parse_expr(rest, error);
        if (!match_punct(*rest, BEE_PUNCT_RPAR)) {
            bee_error_set(error, rest->loc, "unclosed parenthesis, opening pair was at %ld:%ld", par_start.loc.row, par_start.loc.col);
        }

        consume(rest, error);
        return node;
    }

    return bee_parse_path_expr(rest, error);
}

// NOLINTNEXTLINE(misc-no-recursion)
struct bee_ast_node *bee_parse_path_expr(struct bee_token *rest, struct bee_error *error) {
    struct bee_ast_node *node = bee_parse_id_expr(rest, error);
    if (node != NULL && node->tag == BEE_AST_NODE_ID_EXPR) {
        if (match_punct(*rest, BEE_PUNCT_DOT)) {

            // We got a path, so we replace the node with a list containing the path expr
            struct bee_ast_node *list = bee_ast_node_new();
            list->tag = BEE_AST_NODE_PATH_EXPR;
            list->as_array = bee_dynamic_array_new(sizeof(struct bee_ast_node *), 2, 5, bee_ast_node_free);
            if (bee_dynamic_array_push_back(list->as_array, &node) != BEE_DA_STATUS_OK) {
                bee_error_set(error, rest->loc, "could not append an id into a path expr");
                bee_ast_node_free(node);
                return NULL;
            }
            node = list;

            // Append all ids into the path
            while (match_punct(*rest, BEE_PUNCT_DOT)) {
                consume(rest, error);
                if (!match_tag(*rest, BEE_TOKEN_TAG_ID)) {
                    bee_error_set(error, rest->loc, "unexpected token `%*.s` was expecting <id>", rest->len, rest->data);
                    bee_ast_node_free(node);
                    return NULL;
                }

                struct bee_ast_node *id_node = bee_parse_id_expr(rest, error);
                if (bee_error_is_set(error)) {
                    bee_ast_node_free(node);
                    return NULL;
                }

                if (bee_dynamic_array_push_back(list->as_array, &id_node) != BEE_DA_STATUS_OK) {
                    bee_error_set(error, rest->loc, "could not append an id into a path expr");
                    bee_ast_node_free(node);
                    return NULL;
                }
            }
        }
    }

    return node;
}

// NOLINTNEXTLINE(misc-no-recursion)
struct bee_ast_node *bee_parse_id_expr(struct bee_token *rest, struct bee_error *error) {
    if (match_tag(*rest, BEE_TOKEN_TAG_ID)) {
        return bee_ast_node_new_id(consume(rest, error));
    }

    return bee_parse_lit_expr(rest, error);
}

// NOLINTNEXTLINE(misc-no-recursion)
struct bee_ast_node *bee_parse_lit_expr(struct bee_token *rest, struct bee_error *error) {
    if (match_tag(*rest, BEE_TOKEN_TAG_BOOLEAN)
        || match_tag(*rest, BEE_TOKEN_TAG_NUMBER)
        || match_tag(*rest, BEE_TOKEN_TAG_STRING)) {
        struct bee_token token = consume(rest, error);
        struct bee_ast_node *node = bee_ast_node_new_literal(token);
        if (token.tag == BEE_TOKEN_TAG_NUMBER) {
            char digits[BEE_MAX_PARSING_DIGITS];
            if (token.len > BEE_MAX_PARSING_DIGITS) {
                bee_error_set(error, rest->loc, "number has too many digits to parse, max is %d", BEE_MAX_PARSING_DIGITS);
                bee_ast_node_free(node);
                return NULL;
            }

            int8_t sign = *token.data == '-' ? -1 : 1;
            size_t prefix_offset = (sign < 0 ? 1 : 0) + (token.num_base == BEE_NUM_BASE_DEC ? 0 : 2);
            size_t digits_len = token.len - prefix_offset;
            memset(digits, 0L, BEE_MAX_PARSING_DIGITS);
            memcpy(digits, token.data + prefix_offset, digits_len);
            uint8_t base_value[] = {[BEE_NUM_BASE_DEC] = 10,
                    [BEE_NUM_BASE_HEX] = 16,
                    [BEE_NUM_BASE_OCT] = 8,
                    [BEE_NUM_BASE_BIN] = 2};
            switch (token.num_type) {
                case BEE_NUM_TYPE_U8:
                    node->tag = BEE_AST_NODE_LIT_U8_EXPR;
                    node->as_u8 = (uint8_t) (strtoul(digits, NULL, base_value[token.num_base]) * sign);
                    break;
                case BEE_NUM_TYPE_U16:
                    node->tag = BEE_AST_NODE_LIT_U16_EXPR;
                    node->as_u16 = (uint16_t) (
                            strtoul(digits, NULL, base_value[token.num_base]) * sign);
                    break;
                case BEE_NUM_TYPE_U32:
                    node->tag = BEE_AST_NODE_LIT_U32_EXPR;
                    node->as_u32 = (uint32_t) (
                            strtoul(digits, NULL, base_value[token.num_base]) * sign);
                    break;
                case BEE_NUM_TYPE_U64:
                    node->tag = BEE_AST_NODE_LIT_U64_EXPR;
                    node->as_u64 = (uint64_t) (
                            strtoul(digits, NULL, base_value[token.num_base]) * sign);
                    break;
                case BEE_NUM_TYPE_I8:
                    node->tag = BEE_AST_NODE_LIT_I8_EXPR;
                    node->as_i8 =
                            (int8_t) (strtoul(digits, NULL, base_value[token.num_base]) * sign);
                    break;
                case BEE_NUM_TYPE_I16:
                    node->tag = BEE_AST_NODE_LIT_I16_EXPR;
                    node->as_i16 =
                            (int16_t) (strtoul(digits, NULL, base_value[token.num_base]) * sign);
                    break;
                case BEE_NUM_TYPE_I32:
                    node->tag = BEE_AST_NODE_LIT_I32_EXPR;
                    node->as_i32 =
                            (int32_t) (strtoul(digits, NULL, base_value[token.num_base]) * sign);
                    break;
                case BEE_NUM_TYPE_I64:
                    node->tag = BEE_AST_NODE_LIT_I64_EXPR;
                    node->as_i64 =
                            (int64_t) (strtoul(digits, NULL, base_value[token.num_base]) * sign);
                    break;
                case BEE_NUM_TYPE_F32:
                    node->tag = BEE_AST_NODE_LIT_F32_EXPR;
                    node->as_f32 = strtof(digits, NULL) * (float) sign;
                    break;
                case BEE_NUM_TYPE_F64:
                    node->tag = BEE_AST_NODE_LIT_F64_EXPR;
                    node->as_f64 = strtod(digits, NULL) * (double) sign;
                    break;
            }
        } else if (token.tag == BEE_TOKEN_TAG_STRING) {
            node->tag = BEE_AST_NODE_LIT_STR_EXPR;
            node->loc = token.loc;
            node->as_str = calloc(token.len + 1, sizeof(char));
            memcpy(node->as_str, token.data, token.len);
            return node;
        } else if (token.tag == BEE_TOKEN_TAG_BOOLEAN) {
            node->tag = BEE_AST_NODE_LIT_BOL_EXPR;
            node->loc = token.loc;
            node->as_bol = token.len == 4 && memcmp("true", token.data, token.len) == 0;
        }

        return node;
    }

    return NULL;
}
