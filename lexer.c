//
// Created by carlos on 1/19/22.
//

#include "lexer.h"

#include <jit/jit-util.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "utf8.h"

#define min(a, b) (a > b ? b : a)

static_assert(BEE_KEYWORDS_COUNT == 34, "Define keyword string here");
static const char *keywords_table[] = {
        [BEE_KW_MODULE] = "module",
        [BEE_KW_IMPORT] = "import",
        [BEE_KW_EXPORT] = "export",
        [BEE_KW_EXTERN] = "extern",
        [BEE_KW_PRIVATE] = "private",
        [BEE_KW_CONST] = "const",
        [BEE_KW_VAL] = "val",
        [BEE_KW_VAR] = "var",
        [BEE_KW_END] = "end",
        [BEE_KW_STRUCT] = "struct",
        [BEE_KW_ENUM] = "enum",
        [BEE_KW_PROC] = "proc",
        [BEE_KW_PASS] = "pass",
        [BEE_KW_ASSERT] = "assert",
        [BEE_KW_WHILE] = "while",
        [BEE_KW_FOR] = "for",
        [BEE_KW_IN] = "in",
        [BEE_KW_AS] = "as",
        [BEE_KW_LABEL] = "label",
        [BEE_KW_BREAK] = "break",
        [BEE_KW_CONTINUE] = "continue",
        [BEE_KW_IF] = "if",
        [BEE_KW_ELIF] = "elif",
        [BEE_KW_ELSE] = "else",
        [BEE_KW_SWITCH] = "switch",
        [BEE_KW_CASE] = "case",
        [BEE_KW_FALLTHROUGH] = "fallthrough",
        [BEE_KW_DEFAULT] = "default",
        [BEE_KW_TYPE] = "type",
        [BEE_KW_NOT] = "not",
        [BEE_KW_AND] = "and",
        [BEE_KW_OR] = "or",
        [BEE_KW_TRUE] = "true",
        [BEE_KW_FALSE] = "false",
        NULL
};

struct bee_token *bee_token_new() {
    return jit_calloc(1, sizeof(struct bee_token));
}

void bee_token_free(struct bee_token *token) {
    struct bee_token *cur = token;
    struct bee_token *tmp;
    while (cur != NULL) {
        tmp = cur->next;
        jit_free(cur);
        cur = tmp;
    }
}

inline bool is_space(uint32_t codepoint) {
    return codepoint == ' ' || codepoint == '\t';
}

inline bool is_eol(uint32_t codepoint) {
    return codepoint == '\n' || codepoint == ';';
}

inline bool is_digit(uint32_t codepoint) {
    return codepoint >= '0' && codepoint <= '9';
}

inline bool is_alpha(uint32_t codepoint) {
    return (codepoint >= 'a' && codepoint <= 'z') ||
           (codepoint >= 'A' && codepoint <= 'Z') ||
           (codepoint >= 0x2070 && codepoint <= 0x209F) ||
           (codepoint >= 0x0370 && codepoint <= 0x03FF) ||
           codepoint == '_' || codepoint == '$' || codepoint == 0x00B7;
}

inline bool is_alphanum(uint32_t codepoint) {
    return is_digit(codepoint) || is_alpha(codepoint);
}

inline bool is_punct(uint32_t codepoint) {
    return codepoint < 127 && !is_alphanum(codepoint) && !is_space(codepoint) && !is_eol(codepoint);
}

inline bool is_keyword(const char *base, size_t len) {
    const char **cur_keyword = keywords_table;
    while (*cur_keyword != NULL) {
        size_t keyword_len = jit_strlen(*cur_keyword);
        if (jit_memcmp(base, *cur_keyword, len) == 0 && len == keyword_len) {
            return true;
        }
        cur_keyword++;
    }

    return false;
}

void bee_tokenize_buffer(char *data_buf, size_t data_len,
                         struct bee_token *token_start, struct bee_error *error_start) {
    size_t pos = 0L;
    size_t row = 1L;
    size_t col = 0L;
    bool last_escaped = false;
    struct bee_token *cur_token = token_start;
    struct bee_error *cur_error = error_start;
    char *cur_base = data_buf;

    static_assert(BEE_TOKEN_TYPE_COUNT == 7, "Define how to extract token here");
    while (pos < data_len) {
        uint32_t sym = 0L;
        cur_base = data_buf + pos;
        char utf8_seq[4] = {0,0,0,0};

        // TODO: Statically check if seq slot can be filled in
        jit_memcpy(utf8_seq, cur_base, min(4, data_len - pos));
        if (utf8_seq[0] == 0x00) {
            cur_error = bee_error_push(cur_error, col, row,
                                       "buffer is %u bytes long but NULL character has "
                                       "been found at %u bytes", data_len, pos);
            break;
        }

        size_t decoded_bytes = utf8_decode(utf8_seq, &sym);
        if (decoded_bytes == 0) {
            cur_error = bee_error_push(cur_error, col, row, "UTF-8 encoding error");
            decoded_bytes = 1;
            sym = 0xFFFD;
        }
        col += 1;

        if (sym == 0x0000) {
            break;
        }

        if (cur_token->token_type == BEE_TT_STRING) {
            // If we are on a string, keep reading until '"' is reached

            if (sym == '"' && !last_escaped) {
                cur_token->len = cur_base - cur_token->base;
                cur_token->next = bee_token_new();
                cur_token = cur_token->next;
                pos += decoded_bytes; // Continue after (last) '"'
                continue;
            }

            // error: unterminated string
            if (sym == '\n') {
                break;
            }

            if (sym == '\\') {
                last_escaped = true;
            } else {
                last_escaped = false;
            }
        } else if (cur_token->token_type == BEE_TT_DIGITS) {
            // If we are on digits (or word), keep reading numbers until sym is not a number, otherwise fallthrough

            if (!is_digit(sym)) {
                cur_token->len = cur_base - cur_token->base;
                cur_token->next = bee_token_new();
                cur_token = cur_token->next;
                continue; // Continue reading the same sym, but without BEE_TT_DIGITS (or BEE_TT_WORD) context
            }
        } else if (cur_token->token_type == BEE_TT_WORD) {
            // If we are on word, keep reading alpha until sym is not alpha anymore, otherwise fallthrough

            if (!is_alphanum(sym)) {
                cur_token->len = cur_base - cur_token->base;
                cur_token->next = bee_token_new();

                // Change type to keyword if it matches any keyword
                if (is_keyword(cur_token->base, cur_token->len)) {
                    cur_token->token_type = BEE_TT_KEYWORD;
                }

                cur_token = cur_token->next;
                continue; // Continue reading the same sym, but without BEE_TT_WORD context
            }
        } else if (cur_token->token_type == BEE_TT_EOL) {
            // If we are on EOL, keep grouping ending symbols until there is something interesting
            if (is_eol(sym)) {
                if (sym == '\n') {
                    row ++;
                    col = 0L;
                }
            } else {
                cur_token->len = cur_base - cur_token->base;
                cur_token->next = bee_token_new();
                cur_token = cur_token->next;
                continue;
            }
        } else {
            // We are not on any context, respond to whatever comes into sym

            if (sym == '"') {
                // We got a string
                cur_token->token_type = BEE_TT_STRING;
                cur_token->base = cur_base;
                cur_token->col = col;
                cur_token->row = row;
            } else if (is_eol(sym)) {
                cur_token->token_type = BEE_TT_EOL;
                cur_token->base = cur_base;
                cur_token->col = col;
                cur_token->row = row;
            } else if (is_punct(sym)) {
                // We emit punctuation symbols one by one (even if composed) let the parser decide what operand will be used
                cur_token->token_type = BEE_TT_PUNCT;
                cur_token->base = cur_base;
                cur_token->len = 1;
                cur_token->col = col;
                cur_token->row = row;
                cur_token->next = bee_token_new();
                cur_token = cur_token->next;
            } else if (is_alpha(sym)) {
                // We got a keyword/word
                cur_token->token_type = BEE_TT_WORD;
                cur_token->base = cur_base;
                cur_token->col = col;
                cur_token->row = row;
            } else if (is_digit(sym)) {
                // We got digits
                cur_token->token_type = BEE_TT_DIGITS;
                cur_token->base = cur_base;
                cur_token->col = col;
                cur_token->row = row;
            } else if (!is_space(sym) && sym != 0xFFFD) {
                // We don't know what is this
                char err_sym_data[4];
                uint8_t err_sym_size = utf8_encode(sym, err_sym_data);
                cur_error = bee_error_push(cur_error, col, row, "unknown token: U+%d ('%.*s')", sym, err_sym_size, err_sym_data);
            }
        }

        pos += decoded_bytes;
    }

    if (cur_token->token_type == BEE_TT_STRING) {
        bee_error_push(cur_error, col, row, "unterminated string");
    } else if (cur_token->token_type != BEE_TT_EOF) {
        cur_token->len = (cur_base - cur_token->base) + 1;
        if (cur_token->token_type == BEE_TT_WORD && is_keyword(cur_token->base, cur_token->len)) {
            cur_token->token_type = BEE_TT_KEYWORD;
        }

        cur_token->next = bee_token_new();
        cur_token = cur_token->next;
        cur_token->token_type = BEE_TT_EOF;
        cur_token->base = data_buf + pos;
        cur_token->col = col;
        cur_token->row = row;
    }
}