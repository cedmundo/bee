//
// Created by carlos on 1/19/22.
//

#include "lexer.h"

#include <jit/jit-util.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "utf8.h"
#include "error.h"

static_assert(BEE_KEYWORDS_COUNT == 34, "Define keyword string here");
static const char *bee_keywords_table[] = {
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

static_assert(BEE_PUNCTS_COUNT == 44, "Define punct operands here");
static const char *bee_punct_table[] = {
        [BEE_PN_EXPANSION] = "...",
        [BEE_PN_SELF_ADD] = "+=",
        [BEE_PN_SELF_SUB] = "-=",
        [BEE_PN_SELF_DIV] = "/=",
        [BEE_PN_SELF_MUL] = "*=",
        [BEE_PN_SELF_REM] = "%=",
        [BEE_PN_SELF_BIT_LSH] = "<<=",
        [BEE_PN_SELF_BIT_RSH] = ">>=",
        [BEE_PN_SELF_BIT_AND] = "&=",
        [BEE_PN_SELF_BIT_OR] = "|=",
        [BEE_PN_SELF_BIT_NOT] = "~=",
        [BEE_PN_WALRUS] = ":=",
        [BEE_PN_TYPE_ARROW] = "->",
        [BEE_PN_LAMBDA_ARROW] = "=>",
        [BEE_PN_BIT_LSH] = "<<",
        [BEE_PN_BIT_RSH] = ">>",
        [BEE_PN_LOG_AND] = "&&",
        [BEE_PN_LOG_OR] = "||",
        [BEE_PN_EQ] = "==",
        [BEE_PN_NE] = "!=",
        [BEE_PN_LE] = "<=",
        [BEE_PN_GE] = ">=",
        [BEE_PN_PAR_OPEN] = "(",
        [BEE_PN_PAR_CLOSE] = ")",
        [BEE_PN_SBR_OPEN] = "[",
        [BEE_PN_SBR_CLOSE] = "]",
        [BEE_PN_CBR_OPEN] = "{",
        [BEE_PN_CBR_CLOSE] = "}",
        [BEE_PN_SEMICOLON] = ";",
        [BEE_PN_COLON] = ":",
        [BEE_PN_DOT] = ".",
        [BEE_PN_ADD] = "+",
        [BEE_PN_SUB] = "-",
        [BEE_PN_DIV] = "/",
        [BEE_PN_MUL] = "*",
        [BEE_PN_REM] = "%",
        [BEE_PN_LOG_NOT] = "!",
        [BEE_PN_BIT_AND] = "&",
        [BEE_PN_BIT_OR] = "|",
        [BEE_PN_BIT_NOT] = "~",
        [BEE_PN_BIT_XOR] = "^",
        [BEE_PN_LT] = "<",
        [BEE_PN_GT] = ">",
        [BEE_PN_ASSIGN] = "=",
        NULL
};

inline bool bee_is_space(uint32_t codepoint) {
    return codepoint == ' ' || codepoint == '\t';
}

inline bool bee_is_eol(uint32_t codepoint) {
    return codepoint == '\n' || codepoint == ';';
}

inline bool bee_is_digit(uint32_t codepoint) {
    return codepoint >= '0' && codepoint <= '9';
}

inline bool bee_is_alpha(uint32_t codepoint) {
    return (codepoint >= 'a' && codepoint <= 'z') ||
           (codepoint >= 'A' && codepoint <= 'Z') ||
           (codepoint >= 0x2070 && codepoint <= 0x209F) ||
           (codepoint >= 0x0370 && codepoint <= 0x03FF) ||
           codepoint == '_' || codepoint == '$' || codepoint == 0x00B7;
}

inline bool bee_is_alphanum(uint32_t codepoint) {
    return bee_is_digit(codepoint) || bee_is_alpha(codepoint);
}

inline bool bee_is_punct(uint32_t codepoint) {
    return codepoint < 127 && !bee_is_alphanum(codepoint) && !bee_is_space(codepoint) && !bee_is_eol(codepoint);
}

inline bool bee_is_keyword(const char *base, size_t len) {
    const char **cur_keyword = bee_keywords_table;
    while (*cur_keyword != NULL) {
        size_t keyword_len = jit_strlen(*cur_keyword);
        if (jit_memcmp(base, *cur_keyword, len) == 0 && len == keyword_len) {
            return true;
        }
        cur_keyword++;
    }

    return false;
}

void read_codepoint(uint32_t *sym, size_t *decoded_bytes, const char *base,
                           size_t row, size_t col, struct bee_error_list *errors) {
    char utf8_seq[4] = {0,0,0,0};
    for (int i=0;i<4;i++) {
        if (base[i] != 0) {
            utf8_seq[i] = base[i];
        } else {
            break;
        }
    }

    if (utf8_seq[0] == 0x00) {
        *sym = 0x0000;
    }

    *decoded_bytes = utf8_decode(utf8_seq, sym);
    if (*decoded_bytes == 0) {
        bee_error_list_push(errors, row, col, "UTF-8 encoding error");
        *decoded_bytes = 1;
        *sym = 0xFFFD;
    }
}

struct bee_token bee_token_start(char *base, struct bee_error_list *errors) {
    return (struct bee_token) {
        .token_type = BEE_TT_EOF,
        .base = base,
        .len = 0L,
        .row = 1L,
        .col = 0L,
        .errors = errors,
    };
}

bool bee_token_consume_type(struct bee_token *token, enum bee_token_type token_type) {
    struct bee_token ahead = bee_token_consume_any(*token);
    if (ahead.token_type == token_type) {
        *token = ahead;
        return true;
    }

    return false;
}

bool bee_token_consume_keyword(struct bee_token *token, enum bee_keyword keyword) {
    struct bee_token ahead = bee_token_consume_any(*token);
    const char* keyword_str = bee_keywords_table[keyword];
    size_t keyword_len = jit_strlen(keyword_str);
    if (ahead.token_type == BEE_TT_KEYWORD &&
        keyword_len == ahead.len &&
        jit_memcmp(keyword_str, ahead.base, keyword_len) == 0) {
        *token = ahead;
        return true;
    }

    return false;
}

bool bee_token_consume_punct(struct bee_token *token, enum bee_punct punct) {
    struct bee_token ahead = bee_token_consume_any(*token);
    const char* punct_str = bee_punct_table[punct];
    size_t punct_len = jit_strlen(punct_str);
    if (ahead.token_type == BEE_TT_PUNCT &&
        punct_len == ahead.len &&
        jit_memcmp(punct_str, ahead.base, punct_len) == 0) {
        *token = ahead;
        return true;
    }

    return false;
}

struct bee_token bee_token_consume_any(struct bee_token last_token) {
    struct bee_error_list *errors = last_token.errors;
    enum bee_token_type token_type = BEE_TT_EOF;
    size_t len = 0L;
    size_t row = last_token.row;
    size_t col = last_token.col;
    size_t decoded_bytes = 0L;
    uint32_t sym = 0L;

    struct bee_token eof = {
            .token_type=token_type,
            .base=last_token.base,
            .len=len,
            .row=row,
            .col=col,
            .errors=errors
    };

    size_t offset = 0L;
    do {
        read_codepoint(&sym, &decoded_bytes,
                       last_token.base + last_token.len + offset, row, col, errors);

        if (sym == 0x0000 || (!bee_is_space(sym) && sym != 0xFFFD)) {
            break;
        }

        col += 1;
        offset += decoded_bytes;
    } while (true);
    char *base = last_token.base + last_token.len + offset;

    static_assert(BEE_TOKEN_TYPE_COUNT == 7, "Add token consumption algorithm here");
    if (sym == 0x0000) {
        return eof;
    } if (sym == '"') {
        token_type = BEE_TT_STRING;
        len = 2L;
        bool last_escaped = false;
        do {
            read_codepoint(&sym, &decoded_bytes, base + len, row, col, errors);

            if (sym == 0x0000 || sym == '\n') {
                bee_error_list_push(errors, row, col,"unterminated string literal");
                return eof;
            }

            if (sym == '\\') {
                last_escaped = true;
            } else {
                last_escaped = false;
            }

            if (sym == '"' && !last_escaped) {
                break;
            }

            col += 1;
            len += decoded_bytes;
        } while(true);
        len = len + 1;
    } else if (bee_is_eol(sym)) {
        token_type = BEE_TT_EOL;
        do {
            read_codepoint(&sym, &decoded_bytes, base + len, row, col, errors);

            if (sym == '\n') {
                row ++;
                col = 0L;
            }

            if (sym == 0x0000 || !bee_is_eol(sym)) {
                break;
            }

            len += decoded_bytes;
        } while(true);
    } else if (bee_is_punct(sym)) {
        token_type = BEE_TT_PUNCT;
        for (const char **comparing_punct = bee_punct_table; comparing_punct != NULL; comparing_punct++) {
            size_t comparing_punct_len = jit_strlen(*comparing_punct);
            if (jit_memcmp(*comparing_punct, base, comparing_punct_len) == 0) {
                len = comparing_punct_len;
                col += comparing_punct_len;
                break;
            }
        }

        if (len == 0L) {
            bee_error_list_push(errors, row, col, "unknown operand");
            return eof;
        }
    } else if (bee_is_alpha(sym)) {
        do {
            read_codepoint(&sym, &decoded_bytes, base + len, row, col, errors);
            if (sym == 0x0000 || !bee_is_alphanum(sym)) {
                break;
            }

            col += 1;
            len += decoded_bytes;
        } while(true);

        if (bee_is_keyword(base, len)) {
            token_type = BEE_TT_KEYWORD;
        } else {
            token_type = BEE_TT_WORD;
        }
    } else if (bee_is_digit(sym)) {
        token_type = BEE_TT_DIGITS;
        do {
            read_codepoint(&sym, &decoded_bytes, base + len, row, col, errors);
            if (sym == 0x0000 || !bee_is_digit(sym)) {
                break;
            }

            col += 1;
            len += decoded_bytes;
        } while(true);
    } else {
        char err_sym_data[4];
        uint8_t err_sym_size = utf8_encode(sym, err_sym_data);
        bee_error_list_push(errors, row, col,
                            "unknown token: U+%d ('%.*s')", sym, err_sym_size, err_sym_data);
        return eof;
    }

    return (struct bee_token) {
            .token_type=token_type,
            .base=base,
            .len=len,
            .row=row,
            .col=col,
            .errors=errors
    };
}
