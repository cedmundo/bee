//
// Created by carlos on 6/25/22.
//
#include <stdbool.h>
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

void bee_ast_node_free(struct bee_ast_node *node) {
    switch(node->type) {
        case BEE_AST_NODE_ID:
            jit_free(node->as_str);
            break;
    }
}

struct bee_ast_node *bee_parse_expr(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_bit_log_or(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_bit_log_and(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_bit_log_not(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_bit_rel(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_bit_or(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_bit_xor(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_bit_and(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_shift(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_add(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_mul(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_unary(struct bee_token *rest, struct bee_parser_error *error) {}

struct bee_ast_node *bee_parse_primary(struct bee_token *rest, struct bee_parser_error *error) {
    if (match_punct_and_consume(rest, BEE_PUNCT_LP)) {
        struct bee_ast_node *node = bee_parse_expr(rest, error);
        if (!match_punct_and_consume(rest, BEE_PUNCT_RP)) {
            error->type = BEE_PARSER_ERROR_UNEXPECTED_TOKEN;
            error->token = *rest;
            jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: `)`", rest->len, rest->base + rest->off);
            return NULL;
        }

        return node;
    } else if (match_type(*rest, BEE_TOKEN_TYPE_ID)) {
        struct bee_token id_data = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_ID;
        node->token = id_data;
        node->as_str = jit_calloc(id_data.len+1, sizeof(char));
        jit_memcpy(node->as_str, id_data.base + id_data.off, id_data.len);
    } else if (match_type(*rest, BEE_TOKEN_TYPE_BOOLEAN)) {
        struct bee_token bol_data = consume(rest);
        struct bee_ast_node *node = bee_ast_node_new();
        node->type = BEE_AST_NODE_LIT_BOL;
        node->token = bol_data;
        node->as_bol = bol_data.keyword_type == BEE_KEYWORD_TRUE;
    } else if (match_type(*rest, BEE_TOKEN_TYPE_NUMBER)) {
    } else if (match_type(*rest, BEE_TOKEN_TYPE_STRING)) {
    }

    error->type = BEE_PARSER_ERROR_UNEXPECTED_TOKEN;
    error->token = *rest;
    jit_sprintf(error->msg, "Unexpected token `%.*s`, was expecting: `(`<expr>`)`, <id>, <num>, <str> or <bol>", rest->len, rest->base + rest->off);
    return NULL;
}