//
// Created by carlos on 7/17/22.
//

#ifndef BEE_BERR_H
#define BEE_BERR_H
#include <stddef.h>
#include <stdbool.h>

enum bee_error_type {
    BEE_ERROR_NONE,
    BEE_ERROR_LEXER_UNKNOWN_CHAR,
    BEE_ERROR_LEXER_UNTERMINATED_STR,
    BEE_ERROR_LEXER_UNKNOWN_PUNCT,
    BEE_ERROR_LEXER_INVALID_BASE,
    BEE_ERROR_PARSER_WAS_EXPECTING_EXPR,
    BEE_ERROR_PARSER_WAS_EXPECTING_ID,
    BEE_ERROR_PARSER_WAS_EXPECTING_IN,
    BEE_ERROR_PARSER_WAS_EXPECTING_ASSIGN,
    BEE_ERROR_PARSER_WAS_EXPECTING_COMMA,
    BEE_ERROR_PARSER_WAS_EXPECTING_RPAR,
    BEE_ERROR_TYPES,
};

struct bee_error {
    const char *filename;
    char msg[500];
    size_t row;
    size_t col;
    enum bee_error_type type;
};

void bee_set_no_error(struct bee_error *error, const char *filename);
void bee_set_error(struct bee_error *error, enum bee_error_type type,
        const char *filename, size_t row, size_t col, const char *fmt, ...);
void bee_set_error_type(struct bee_error *error, enum bee_error_type type, const char *fmt, ...);

bool bee_has_error(struct bee_error *error);
void bee_print_error(struct bee_error *error);

#endif //BEE_BERR_H
