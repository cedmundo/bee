//
// Created by carlos on 6/25/22.
//
#include <stdbool.h>
#include <jit/jit-util.h>
#include <stdlib.h>

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

void bee_ast_node_free(struct bee_ast_node *node) {
    switch(node->type) {
        case BEE_AST_NODE_LIT_STR:
        case BEE_AST_NODE_ID:
            if (node->as_str != NULL) {
                jit_free(node->as_str);
            }
            break;
        case BEE_AST_NODE_UNA_DEREF:
        case BEE_AST_NODE_UNA_BIT_NEG:
        case BEE_AST_NODE_UNA_ARI_POS:
        case BEE_AST_NODE_UNA_ARI_NEG:
        case BEE_AST_NODE_UNA_LOG_NEG:
        case BEE_AST_NODE_UNA_REF:
            if (node->left != NULL) {
                bee_ast_node_free(node->left);
            }
            break;
    }

    jit_free(node);
}

struct bee_ast_node *bee_parse_expr(struct bee_token *rest, struct bee_parser_error *error) {
    return bee_parse_unary(rest, error);
}

// struct bee_ast_node *bee_parse_bit_log_or(struct bee_token *rest, struct bee_parser_error *error) {}
//
// struct bee_ast_node *bee_parse_bit_log_and(struct bee_token *rest, struct bee_parser_error *error) {}
//
// struct bee_ast_node *bee_parse_bit_log_not(struct bee_token *rest, struct bee_parser_error *error) {}
//
// struct bee_ast_node *bee_parse_bit_rel(struct bee_token *rest, struct bee_parser_error *error) {}
//
// struct bee_ast_node *bee_parse_bit_or(struct bee_token *rest, struct bee_parser_error *error) {}
//
// struct bee_ast_node *bee_parse_bit_xor(struct bee_token *rest, struct bee_parser_error *error) {}
//
// struct bee_ast_node *bee_parse_bit_and(struct bee_token *rest, struct bee_parser_error *error) {}
//
// struct bee_ast_node *bee_parse_shift(struct bee_token *rest, struct bee_parser_error *error) {}
//
// struct bee_ast_node *bee_parse_add(struct bee_token *rest, struct bee_parser_error *error) {}
//
// struct bee_ast_node *bee_parse_mul(struct bee_token *rest, struct bee_parser_error *error) {}

// unary = primary | ('+' | '-' | '*' | '&' | '~') unary
struct bee_ast_node *bee_parse_unary(struct bee_token *rest, struct bee_parser_error *error) {
    if (match_punct(*rest, BEE_PUNCT_ADD)) {
        struct bee_token token = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_UNA_ARI_POS;
        node->token = token;
        node->left = bee_parse_unary(rest, error);
        return node;
    }

    if (match_punct(*rest, BEE_PUNCT_SUB)) {
        struct bee_token token = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_UNA_ARI_NEG;
        node->token = token;
        node->left = bee_parse_unary(rest, error);
        return node;
    }

    if (match_punct(*rest, BEE_PUNCT_MUL)) {
        struct bee_token token = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_UNA_DEREF;
        node->token = token;
        node->left = bee_parse_unary(rest, error);
        return node;
    }

    if (match_punct(*rest, BEE_PUNCT_BIT_AND)) {
        struct bee_token token = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_UNA_REF;
        node->token = token;
        node->left = bee_parse_unary(rest, error);
        return node;
    }

    if (match_punct(*rest, BEE_PUNCT_BIT_NEG)) {
        struct bee_token token = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_UNA_BIT_NEG;
        node->token = token;
        node->left = bee_parse_unary(rest, error);
        return node;
    }

    return bee_parse_primary(rest, error);
}

// primary = '(' expr ')' | id | lit
struct bee_ast_node *bee_parse_primary(struct bee_token *rest, struct bee_parser_error *error) {
    if (match_punct_and_consume(rest, BEE_PUNCT_LPAR)) {
        struct bee_ast_node *node = bee_parse_expr(rest, error);
        if (!match_punct_and_consume(rest, BEE_PUNCT_RPAR)) {
            error->type = BEE_PARSER_ERROR_UNEXPECTED_TOKEN;
            error->token = *rest;
            jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: `)`", rest->len, rest->data + rest->off);
            return NULL;
        }

        return node;
    } else if (match_type(*rest, BEE_TOKEN_TYPE_ID)) {
        struct bee_token id_data = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_ID;
        node->token = id_data;
        node->as_str = jit_calloc(id_data.len+1, sizeof(char));
        jit_memcpy(node->as_str, id_data.data + id_data.off, id_data.len);
        return node;
    } else if (match_type(*rest, BEE_TOKEN_TYPE_BOOLEAN)) {
        struct bee_token bol_data = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_LIT_BOL;
        node->token = bol_data;
        node->as_bol = bol_data.keyword_type == BEE_KEYWORD_TRUE;
        return node;
    } else if (match_type(*rest, BEE_TOKEN_TYPE_NUMBER)) {
        struct bee_token num_data = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->token = num_data;
        int8_t neg = *(num_data.data + num_data.off) == '-' ? -1 : 1;
        const char *digits = num_data.data + num_data.off + (num_data.num_base == BEE_NUM_BASE_DEC ? (neg < 0 ? 1 : 0) : 2);
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
                node->as_f32 = strtof(digits, NULL) * (float)neg;
                break;
            case BEE_NUM_TYPE_F64:
                node->type = BEE_AST_NODE_LIT_F64;
                node->as_f64 = strtod(digits, NULL) * (double)neg;
                break;
        }
        return node;
    } else if (match_type(*rest, BEE_TOKEN_TYPE_STRING)) {
        struct bee_token str_data = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_LIT_STR;
        node->token = str_data;
        node->as_str = jit_calloc(str_data.len + 1, sizeof(char));
        jit_memcpy(node->as_str, str_data.data + str_data.off, str_data.len);
        return node;
    }

    error->type = BEE_PARSER_ERROR_UNEXPECTED_TOKEN;
    error->token = *rest;
    jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: `(`<expr>`)`, <id>, <num>, <str> or <bol>",
                rest->len, rest->data + rest->off);
    return NULL;
}