//
// Created by carlos on 6/25/22.
//
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <jit/jit-util.h>

#include "lexer.h"
#include "parser.h"

bool match_type(struct bee_token token, enum bee_token_type token_type) {
    return token.type == token_type;
}

bool match_punct(struct bee_token token, enum bee_punct_type punct_type) {
    return match_type(token, BEE_TOKEN_TYPE_PUNCT) && token.punct_type == punct_type;
}

bool match_keyword(struct bee_token token, enum bee_keyword_type keyword_type) {
    return match_type(token, BEE_TOKEN_TYPE_KEYWORD) && token.keyword_type == keyword_type;
}

struct bee_token consume(struct bee_token *rest) {
    struct bee_token current = *rest;
    *rest = bee_token_next(current);
    return current;
}

bool match_type_and_consume(struct bee_token *rest, enum bee_token_type token_type) {
    if (match_type(*rest, token_type)) {
        consume(rest);
        return true;
    }

    return false;
}

bool match_punct_and_consume(struct bee_token *rest, enum bee_punct_type punct_type) {
    if (match_punct(*rest, punct_type)) {
        consume(rest);
        return true;
    }

    return false;
}

bool match_keyword_and_consume(struct bee_token *rest, enum bee_keyword_type keyword_type) {
    if (match_keyword(*rest, keyword_type)) {
        consume(rest);
        return true;
    }

    return false;
}

struct bee_ast_node *bee_ast_node_new() {
    return jit_calloc(1, sizeof(struct bee_ast_node));
}

struct bee_ast_node *bee_ast_node_new_unary(struct bee_token at_token,
                                            enum bee_ast_node_type type, struct bee_ast_node *left) {
    assert(type >= BEE_AST_NODE_UNA_LOG_NEG && type <= BEE_AST_NODE_UNA_ARI_POS);
    struct bee_ast_node *node = bee_ast_node_new();
    node->type = type;
    node->filename = at_token.name;
    node->row = at_token.row;
    node->col = at_token.col;
    node->left = left;
    return node;
}

struct bee_ast_node *bee_ast_node_new_binary(struct bee_token at_token, enum bee_ast_node_type type,
                                             struct bee_ast_node *left, struct bee_ast_node *right) {
    assert(type >= BEE_AST_NODE_BIN_ADD && type <= BEE_AST_NODE_TYPES);
    struct bee_ast_node *node = bee_ast_node_new();
    node->type = type;
    node->filename = at_token.name;
    node->row = at_token.row;
    node->col = at_token.col;
    node->left = left;
    node->right = right;
    return node;
}

void bee_ast_node_free(struct bee_ast_node *node) {
    switch (node->type) {
        case BEE_AST_NODE_LIT_STR:
        case BEE_AST_NODE_ID:
            if (node->as_str != NULL) {
                jit_free(node->as_str);
            }
            break;
        case BEE_AST_NODE_UNA_BIT_NEG:
        case BEE_AST_NODE_UNA_ARI_POS:
        case BEE_AST_NODE_UNA_ARI_NEG:
        case BEE_AST_NODE_UNA_LOG_NEG:
            if (node->left != NULL) {
                bee_ast_node_free(node->left);
            }
            break;
        case BEE_AST_NODE_BIN_ADD:
        case BEE_AST_NODE_BIN_SUB:
        case BEE_AST_NODE_BIN_MUL:
        case BEE_AST_NODE_BIN_DIV:
        case BEE_AST_NODE_BIN_REM:
        case BEE_AST_NODE_BIN_EQ:
        case BEE_AST_NODE_BIN_NE:
        case BEE_AST_NODE_BIN_GT:
        case BEE_AST_NODE_BIN_GE:
        case BEE_AST_NODE_BIN_LT:
        case BEE_AST_NODE_BIN_LE:
        case BEE_AST_NODE_BIN_LSH:
        case BEE_AST_NODE_BIN_RSH:
        case BEE_AST_NODE_BIN_BIT_AND:
        case BEE_AST_NODE_BIN_BIT_OR:
        case BEE_AST_NODE_BIN_BIT_XOR:
        case BEE_AST_NODE_BIN_LOG_AND:
        case BEE_AST_NODE_BIN_LOG_OR:
        case BEE_AST_NODE_BIN_DUCK_ASSIGN:
        case BEE_AST_NODE_LET_IN_EXPR:
        case BEE_AST_NODE_ARG:
            if (node->left != NULL) {
                bee_ast_node_free(node->left);
            }

            if (node->right != NULL) {
                bee_ast_node_free(node->right);
            }
            break;
        case BEE_AST_NODE_NONE:
        case BEE_AST_NODE_LIT_BOL:
        case BEE_AST_NODE_LIT_I8:
        case BEE_AST_NODE_LIT_I16:
        case BEE_AST_NODE_LIT_I32:
        case BEE_AST_NODE_LIT_I64:
        case BEE_AST_NODE_LIT_U8:
        case BEE_AST_NODE_LIT_U16:
        case BEE_AST_NODE_LIT_U32:
        case BEE_AST_NODE_LIT_U64:
        case BEE_AST_NODE_LIT_F32:
        case BEE_AST_NODE_LIT_F64:
            break;
    }

    jit_free(node);
}

struct bee_ast_node *bee_parse_expr(struct bee_token *rest, struct bee_parser_error *error) {
    return bee_parse_let_in(rest, error);
}

// let_in = log_or | 'let' id ':=' expr 'in' expr
struct bee_ast_node *bee_parse_let_in(struct bee_token *rest, struct bee_parser_error *error) {
    if (match_keyword(*rest, BEE_KEYWORD_LET)) {
        struct bee_token let_starting_at = consume(rest);
        struct bee_ast_node *id_node = bee_must_parse_id(rest, error);
        if (error->type != BEE_PARSER_ERROR_NONE) {
            if (id_node != NULL) {
                bee_ast_node_free(id_node);
            }
            return NULL;
        }

        if (!match_punct(*rest, BEE_PUNCT_WALRUS)) {
            if (id_node != NULL) {
                bee_ast_node_free(id_node);
            }
            error->type = BEE_PARSER_ERROR_WAS_EXPECTING_ASSIGN;
            error->filename = rest->name;
            error->row = rest->row;
            error->col = rest->col;
            jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: `:=`",
                        rest->len, rest->data + rest->off);
            return NULL;
        }
        struct bee_token walrus_starting_at = consume(rest);

        struct bee_ast_node *value_expr_node = bee_parse_expr(rest, error);
        if (error->type != BEE_PARSER_ERROR_NONE) {
            if (id_node != NULL) {
                bee_ast_node_free(id_node);
            }
            if (value_expr_node != NULL) {
                bee_ast_node_free(value_expr_node);
            }
            return NULL;
        }

        if (!match_keyword_and_consume(rest, BEE_KEYWORD_IN)) {
            if (id_node != NULL) {
                bee_ast_node_free(id_node);
            }
            if (value_expr_node != NULL) {
                bee_ast_node_free(value_expr_node);
            }
            error->type = BEE_PARSER_ERROR_WAS_EXPECTING_IN;
            error->filename = rest->name;
            error->row = rest->row;
            error->col = rest->col;
            jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: `in`",
                        rest->len, rest->data + rest->off);
            return NULL;
        }

        struct bee_ast_node *into_expr_node = bee_parse_expr(rest, error);
        if (error->type != BEE_PARSER_ERROR_NONE) {
            if (id_node != NULL) {
                bee_ast_node_free(id_node);
            }
            if (value_expr_node != NULL) {
                bee_ast_node_free(value_expr_node);
            }
            if (into_expr_node != NULL) {
                bee_ast_node_free(into_expr_node);
            }
            return NULL;
        }

        struct bee_ast_node *assign_node = bee_ast_node_new_binary(walrus_starting_at,
                                                                   BEE_AST_NODE_BIN_DUCK_ASSIGN, id_node, value_expr_node);
        struct bee_ast_node *let_in_node = bee_ast_node_new_binary(let_starting_at,
                                                                   BEE_AST_NODE_LET_IN_EXPR, assign_node, into_expr_node);
        return let_in_node;
    }

   return bee_parse_log_or(rest, error);
}

// log_or = log_and ('or' log_and)*
struct bee_ast_node *bee_parse_log_or(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_log_and(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return node;
    }

    for (;;) {
        if (match_keyword(*rest, BEE_KEYWORD_OR)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_LOG_OR, node, bee_parse_log_and(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        break;
    }

    return node;
}

// log_and = log_not ('and' log_not)*
struct bee_ast_node *bee_parse_log_and(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_log_not(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return node;
    }

    for (;;) {
        if (match_keyword(*rest, BEE_KEYWORD_AND)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_LOG_AND, node, bee_parse_log_not(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        break;
    }

    return node;
}

// log_not = rel | 'not' log_not
struct bee_ast_node *bee_parse_log_not(struct bee_token *rest, struct bee_parser_error *error) {
    if (match_keyword(*rest, BEE_KEYWORD_NOT)) {
        struct bee_token token = consume(rest);
        return bee_ast_node_new_unary(token, BEE_AST_NODE_UNA_LOG_NEG, bee_parse_log_not(rest, error));
    }

    return bee_parse_rel(rest, error);
}

// rel = bit_or ('==' bit_or | '!=' bit_or | '>' bit_or | '>=' bit_or
//                  | '<' bit_or | '<=' bit_or)*
struct bee_ast_node *bee_parse_rel(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_bit_or(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return node;
    }

    for (;;) {
        if (match_punct(*rest, BEE_PUNCT_D_EQ)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_EQ, node, bee_parse_bit_or(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        if (match_punct(*rest, BEE_PUNCT_NE)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_NE, node, bee_parse_bit_or(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        if (match_punct(*rest, BEE_PUNCT_GT)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_GT, node, bee_parse_bit_or(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        if (match_punct(*rest, BEE_PUNCT_GE)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_GE, node, bee_parse_bit_or(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        if (match_punct(*rest, BEE_PUNCT_LT)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_LT, node, bee_parse_bit_or(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        if (match_punct(*rest, BEE_PUNCT_LE)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_LE, node, bee_parse_bit_or(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        break;
    }

    return node;
}

// bit_or = bit_xor ('|' bit_xor)*
struct bee_ast_node *bee_parse_bit_or(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_bit_xor(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return node;
    }

    for (;;) {
        if (match_punct(*rest, BEE_PUNCT_VBAR)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_BIT_OR, node, bee_parse_bit_xor(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        break;
    }

    return node;
}

// bit_xor = bit_and ('^' bit_and)*
struct bee_ast_node *bee_parse_bit_xor(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_bit_and(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return node;
    }

    for (;;) {
        if (match_punct(*rest, BEE_PUNCT_CARET)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_BIT_XOR, node, bee_parse_bit_and(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        break;
    }

    return node;
}

// bit_and = shift ('&' shift)*
struct bee_ast_node *bee_parse_bit_and(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_shift(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return node;
    }

    for (;;) {
        if (match_punct(*rest, BEE_PUNCT_AMPERSAND)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_BIT_AND, node, bee_parse_shift(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        break;
    }

    return node;
}

// shift = add ('>>' add | '<<' add)*
struct bee_ast_node *bee_parse_shift(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_add(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return node;
    }

    for (;;) {
        if (match_punct(*rest, BEE_PUNCT_D_LT)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_LSH, node, bee_parse_add(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        if (match_punct(*rest, BEE_PUNCT_D_GT)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_RSH, node, bee_parse_add(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        break;
    }

    return node;
}

// add = mul ('+' mul | '-' mul)*
struct bee_ast_node *bee_parse_add(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_mul(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return node;
    }

    for (;;) {
        if (match_punct(*rest, BEE_PUNCT_PLUS)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_ADD, node, bee_parse_mul(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        if (match_punct(*rest, BEE_PUNCT_MINUS)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_SUB, node, bee_parse_mul(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        break;
    }

    return node;
}

// mul = unary ('*' unary | '/' unary | '%' unary)*
struct bee_ast_node *bee_parse_mul(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_unary(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return node;
    }

    for (;;) {
        if (match_punct(*rest, BEE_PUNCT_ASTERISK)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_MUL, node, bee_parse_unary(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        if (match_punct(*rest, BEE_PUNCT_SLASH)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_DIV, node, bee_parse_unary(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        if (match_punct(*rest, BEE_PUNCT_PERCENT)) {
            struct bee_token token = consume(rest);
            node = bee_ast_node_new_binary(token, BEE_AST_NODE_BIN_REM, node, bee_parse_unary(rest, error));
            if (error->type != BEE_PARSER_ERROR_NONE) {
                return node;
            }
            continue;
        }

        break;
    }

    return node;
}

// unary = call_or_lit | ('+' | '-' | '~') unary
struct bee_ast_node *bee_parse_unary(struct bee_token *rest, struct bee_parser_error *error) {
    if (match_punct(*rest, BEE_PUNCT_PLUS)) {
        struct bee_token token = consume(rest);
        return bee_ast_node_new_unary(token, BEE_AST_NODE_UNA_ARI_POS, bee_parse_unary(rest, error));
    }

    if (match_punct(*rest, BEE_PUNCT_MINUS)) {
        struct bee_token token = consume(rest);
        return bee_ast_node_new_unary(token, BEE_AST_NODE_UNA_ARI_NEG, bee_parse_unary(rest, error));
    }

    if (match_punct(*rest, BEE_PUNCT_TILDE)) {
        struct bee_token token = consume(rest);
        return bee_ast_node_new_unary(token, BEE_AST_NODE_UNA_BIT_NEG, bee_parse_unary(rest, error));
    }

    return bee_parse_call_or_lit(rest, error);
}

// call_or_lit = lit | call
struct bee_ast_node *bee_parse_call_or_lit(struct bee_token *rest, struct bee_parser_error *error) {
    if (match_type(*rest, BEE_TOKEN_TYPE_NUMBER)) {
        struct bee_token num_data = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->filename = num_data.name;
        node->row = num_data.row;
        node->col = num_data.col;
        int8_t neg = *(num_data.data + num_data.off) == '-' ? -1 : 1;
        const char *digits =
                num_data.data + num_data.off + (num_data.num_base == BEE_NUM_BASE_DEC ? (neg < 0 ? 1 : 0) : 2);
        int base_value[] = {[BEE_NUM_BASE_DEC] = 10, [BEE_NUM_BASE_HEX] = 16,
                [BEE_NUM_BASE_OCT] = 8, [BEE_NUM_BASE_BIN] = 2};
        switch (num_data.num_type) {
            case BEE_NUM_TYPE_U8:
                node->type = BEE_AST_NODE_LIT_U8;
                node->as_u8 = (uint8_t) (strtoul(digits, NULL, base_value[num_data.num_base]) * neg);
                break;
            case BEE_NUM_TYPE_U16:
                node->type = BEE_AST_NODE_LIT_U16;
                node->as_u16 = (uint16_t) (strtoul(digits, NULL, base_value[num_data.num_base]) * neg);
                break;
            case BEE_NUM_TYPE_U32:
                node->type = BEE_AST_NODE_LIT_U32;
                node->as_u32 = (uint32_t) (strtoul(digits, NULL, base_value[num_data.num_base]) * neg);
                break;
            case BEE_NUM_TYPE_U64:
                node->type = BEE_AST_NODE_LIT_U64;
                node->as_u64 = (uint64_t) (strtoul(digits, NULL, base_value[num_data.num_base]) * neg);
                break;
            case BEE_NUM_TYPE_I8:
                node->type = BEE_AST_NODE_LIT_I8;
                node->as_i8 = (int8_t) (strtoul(digits, NULL, base_value[num_data.num_base]) * neg);
                break;
            case BEE_NUM_TYPE_I16:
                node->type = BEE_AST_NODE_LIT_I16;
                node->as_i16 = (int16_t) (strtoul(digits, NULL, base_value[num_data.num_base]) * neg);
                break;
            case BEE_NUM_TYPE_I32:
                node->type = BEE_AST_NODE_LIT_I32;
                node->as_i32 = (int32_t) (strtoul(digits, NULL, base_value[num_data.num_base]) * neg);
                break;
            case BEE_NUM_TYPE_I64:
                node->type = BEE_AST_NODE_LIT_I64;
                node->as_i64 = (int64_t) (strtoul(digits, NULL, base_value[num_data.num_base]) * neg);
                break;
            case BEE_NUM_TYPE_F32:
                node->type = BEE_AST_NODE_LIT_F32;
                node->as_f32 = strtof(digits, NULL) * (float) neg;
                break;
            case BEE_NUM_TYPE_F64:
                node->type = BEE_AST_NODE_LIT_F64;
                node->as_f64 = strtod(digits, NULL) * (double) neg;
                break;
        }
        return node;
    } else if (match_type(*rest, BEE_TOKEN_TYPE_STRING)) {
        struct bee_token str_data = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_LIT_STR;
        node->filename = str_data.name;
        node->row = str_data.row;
        node->col = str_data.col;
        node->as_str = jit_calloc(str_data.len + 1, sizeof(char));
        jit_memcpy(node->as_str, str_data.data + str_data.off, str_data.len);
        return node;
    }

    return bee_parse_call(rest, error);
}

// call = primary ('(' args ')')*
struct bee_ast_node *bee_parse_call(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = bee_parse_primary(rest, error);
    if (error->type != BEE_PARSER_ERROR_NONE) {
        return NULL;
    }

    // Chain calls
    struct bee_ast_node *root = NULL;
    for(;;) {
        if (match_punct(*rest, BEE_PUNCT_LPAR)) {
            struct bee_token call_start = consume(rest);
            struct bee_ast_node *args = bee_parse_args(rest, error);
            node = bee_ast_node_new_binary(call_start, BEE_AST_NODE_CALL, node, args);
            if (root == NULL) {
                root = node;
            }

            if (!match_punct_and_consume(rest, BEE_PUNCT_RPAR)) {
                error->type = BEE_PARSER_ERROR_WAS_EXPECTING_RPAR;
                error->filename = rest->name;
                error->row = rest->row;
                error->col = rest->col;
                jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: `)`", rest->len, rest->data + rest->off);
                return NULL;
            }
            continue;
        }

        break;
    }

    if (root != NULL) {
        return root;
    }

    return node;
}

// args = (expr ',')*
struct bee_ast_node *bee_parse_args(struct bee_token *rest, struct bee_parser_error *error) {
    struct bee_ast_node *node = NULL, *root = NULL;

    for(;;) {
        if (match_punct(*rest, BEE_PUNCT_RPAR)) {
            break;
        }

        if (root != NULL && !match_punct_and_consume(rest, BEE_PUNCT_COMMA)) {
            error->type = BEE_PARSER_ERROR_WAS_EXPECTING_COMMA;
            error->filename = rest->name;
            error->row = rest->row;
            error->col = rest->col;
            jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: `,`", rest->len, rest->data + rest->off);
            return NULL;
        }

        if(node == NULL) {
            node = bee_ast_node_new_binary(*rest, BEE_AST_NODE_ARG, bee_parse_expr(rest, error), NULL);
            root = node;
        } else {
            node->right = bee_ast_node_new_binary(*rest, BEE_AST_NODE_ARG, bee_parse_expr(rest, error), NULL);
            node = node->right;
        }
    }

    return root;
}

// primary = '(' expr ')' | id
struct bee_ast_node *bee_parse_primary(struct bee_token *rest, struct bee_parser_error *error) {
    if (match_punct_and_consume(rest, BEE_PUNCT_LPAR)) {
        struct bee_ast_node *node = bee_parse_expr(rest, error);
        if (!match_punct_and_consume(rest, BEE_PUNCT_RPAR)) {
            error->type = BEE_PARSER_ERROR_WAS_EXPECTING_RPAR;
            error->filename = rest->name;
            error->row = rest->row;
            error->col = rest->col;
            jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: `)`", rest->len, rest->data + rest->off);
            return NULL;
        }

        return node;
    } else if (match_type(*rest, BEE_TOKEN_TYPE_ID)) {
        return bee_must_parse_id(rest, error);
    } else if (match_type(*rest, BEE_TOKEN_TYPE_BOOLEAN)) {
        struct bee_token bol_data = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_LIT_BOL;
        node->filename = bol_data.name;
        node->row = bol_data.row;
        node->col = bol_data.col;
        node->as_bol = bol_data.keyword_type == BEE_KEYWORD_TRUE;
        return node;
    }

    error->type = BEE_PARSER_ERROR_WAS_EXPECTING_EXPR;
    error->filename = rest->name;
    error->row = rest->row;
    error->col = rest->col;
    jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting an expression",
                rest->len, rest->data + rest->off);
    return NULL;
}

struct bee_ast_node *bee_must_parse_id(struct bee_token *rest, struct bee_parser_error *error) {
    if (!match_type(*rest, BEE_TOKEN_TYPE_ID)) {
        error->type = BEE_PARSER_ERROR_WAS_EXPECTING_ID;
        error->filename = rest->name;
        error->row = rest->row;
        error->col = rest->col;
        jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: <id>",
                    rest->len, rest->data + rest->off);
        return NULL;
    }

    struct bee_token id_data = consume(rest);
    struct bee_ast_node *node = bee_ast_node_new();
    node->type = BEE_AST_NODE_ID;
    node->filename = id_data.name;
    node->row = id_data.row;
    node->col = id_data.col;
    node->as_str = jit_calloc(id_data.len + 1, sizeof(char));
    jit_memcpy(node->as_str, id_data.data + id_data.off, id_data.len);
    return node;
}
