//
// Created by carlos on 6/10/22.
//
#include <stdbool.h>
#include <assert.h>
#include <jit/jit-util.h>

#include "lexer.h"

#define TRACKED_PUNCT_TYPES 32
static_assert(BEE_PUNCT_TYPES == TRACKED_PUNCT_TYPES,
              "Exhaustive punct string table");
static const char *puncts_table[] = {
        [BEE_PUNCT_NONE] = NULL,
        [BEE_PUNCT_WALRUS] = ":=",
        [BEE_PUNCT_EQ] = "==",
        [BEE_PUNCT_NE] = "!=",
        [BEE_PUNCT_GE] = ">=",
        [BEE_PUNCT_LE] = "<=",
        [BEE_PUNCT_LOG_AND] = "&&",
        [BEE_PUNCT_LOG_OR] = "||",
        [BEE_PUNCT_BIT_RSH] = ">>",
        [BEE_PUNCT_BIT_LSH] = "<<",
        [BEE_PUNCT_RANGE] = "..",
        [BEE_PUNCT_ADD] = "+",
        [BEE_PUNCT_SUB] = "-",
        [BEE_PUNCT_MUL] = "*",
        [BEE_PUNCT_DIV] = "/",
        [BEE_PUNCT_REM] = "%",
        [BEE_PUNCT_BIT_AND] = "&",
        [BEE_PUNCT_BIT_OR] = "|",
        [BEE_PUNCT_AT] = "@",
        [BEE_PUNCT_DOT] = ".",
        [BEE_PUNCT_COMMA] = ",",
        [BEE_PUNCT_LOG_NEG] = "!",
        [BEE_PUNCT_BIT_NEG] = "~",
        [BEE_PUNCT_ARI_NEG] = "-",
        [BEE_PUNCT_ASSIGN] = "=",
        [BEE_PUNCT_COLON] = ":",
        [BEE_PUNCT_LPAR] = "(",
        [BEE_PUNCT_RPAR] = ")",
        [BEE_PUNCT_LSB] = "[",
        [BEE_PUNCT_RSB] = "]",
        [BEE_PUNCT_LCB] = "{",
        [BEE_PUNCT_RCB] = "}",
};

#define TRACKED_KEYWORD_TYPES 33
static_assert(BEE_KEYWORD_TYPES == TRACKED_KEYWORD_TYPES,
              "Exhaustive punct string table");
static const char *keywords_table[] = {
        [BEE_KEYWORD_NONE] = NULL,
        [BEE_KEYWORD_END] = "end",
        [BEE_KEYWORD_MODULE] = "module",
        [BEE_KEYWORD_IMPORT] = "import",
        [BEE_KEYWORD_EXPORT] = "export",
        [BEE_KEYWORD_EXTERN] = "extern",
        [BEE_KEYWORD_FUNCTION] = "function",
        [BEE_KEYWORD_RETURN] = "return",
        [BEE_KEYWORD_ASSERT] = "assert",
        [BEE_KEYWORD_WHILE] = "while",
        [BEE_KEYWORD_FOR] = "for",
        [BEE_KEYWORD_IF] = "if",
        [BEE_KEYWORD_ELIF] = "elif",
        [BEE_KEYWORD_ELSE] = "else",
        [BEE_KEYWORD_IN] = "in",
        [BEE_KEYWORD_AS] = "as",
        [BEE_KEYWORD_IS] = "is",
        [BEE_KEYWORD_SWITCH] = "switch",
        [BEE_KEYWORD_CASE] = "case",
        [BEE_KEYWORD_CONTINUE] = "continue",
        [BEE_KEYWORD_BREAK] = "break",
        [BEE_KEYWORD_FALLTHROUGH] = "fallthrough",
        [BEE_KEYWORD_TYPE] = "type",
        [BEE_KEYWORD_LET] = "let",
        [BEE_KEYWORD_EFFECT] = "effect",
        [BEE_KEYWORD_HANDLER] = "handler",
        [BEE_KEYWORD_HANDLE] = "handle",
        [BEE_KEYWORD_RESUME] = "resume",
        [BEE_KEYWORD_ABORT] = "abort",
        [BEE_KEYWORD_WITH] = "with",
        [BEE_KEYWORD_DO] = "do",
        [BEE_KEYWORD_TRUE] = "true",
        [BEE_KEYWORD_FALSE] = "false",
};


static bool is_eof(const char c) {
    return c == '\0';
}

static bool is_eol(const char c) {
    return c == '\n' || c == '\r';
}

static bool is_digit(const char c, enum bee_num_base base) {
    switch (base) {
        case BEE_NUM_BASE_BIN:
            return c == '0' || c == '1';
        case BEE_NUM_BASE_OCT:
            return c >= '0' && c <= '7';
        case BEE_NUM_BASE_DEC:
            return c >= '0' && c <= '9';
        case BEE_NUM_BASE_HEX:
            return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
        default:
            return false;
    }
}

static bool is_alpha(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool is_space(const char c) {
    return c == ' ' || c == '\t';
}

static bool is_id_start(const char c) {
    return is_alpha(c) || c == '_' || c == '$';
}

static bool is_punct(const char c) {
    return !is_eof(c) && !is_digit(c, BEE_NUM_BASE_DEC) && !is_id_start(c) &&
           !is_space(c) && !is_eol(c);
}

static size_t try_read_eol(const char *s) {
    size_t i;
    for (i = 0; i < jit_strlen(s); i++) {
        if (!is_eol(*(s + i))) {
            break;
        }
    }
    return i;
}

static size_t try_read_spaces(const char *s) {
    size_t i;
    for (i = 0; i < jit_strlen(s); i++) {
        if (!is_space(*(s + i))) {
            break;
        }
    }
    return i;
}

static size_t try_read_id(const char *s) {
    size_t i;
    for (i = 0; i < jit_strlen(s); i++) {
        if (i == 0 && !is_id_start(*s)) {
            return 0;
        }

        if (!(is_alpha(*(s + i)) || is_digit(*(s + i), BEE_NUM_BASE_DEC) || *(s+i) == '_' ||
              *(s+i) == '$')) {
            break;
        }
    }

    return i;
}

static size_t try_read_lit_num(const char *s, bool *is_float, enum bee_num_base *base,
        enum bee_num_type *type, enum bee_token_error *error) {
    size_t i = 0;
    size_t sl = jit_strlen(s);
    *is_float = false;
    *base = BEE_NUM_BASE_DEC;
    *type = BEE_NUM_TYPE_U32;
    *error = BEE_TOKEN_ERROR_NONE;

    if (*s == '-') {
        i += 1;
        *type = BEE_NUM_TYPE_I32;

        bool follows_digit = is_digit(*(s+1), *base);
        bool follows_dot_with_digit = *(s+1) == '.' && is_digit(*(s+2), *base);
        if (!follows_digit && !follows_dot_with_digit) {
            return 0;
        }
    }

    if (jit_strncmp("0x", s+i, 2) == 0) {
        i += 2;
        *base = BEE_NUM_BASE_HEX;
    } else if (jit_strncmp("0b", s+i, 2) == 0) {
        i += 2;
        *base = BEE_NUM_BASE_BIN;
    } else if (jit_strncmp("0o", s+i, 2) == 0) {
        i += 2;
        *base = BEE_NUM_BASE_OCT;
    }

    for (; i < sl; i++) {
        if (!is_digit(*(s + i), *base)) {
            break;
        }
    }

    if (*(s + i) == '.') {
        if (*base != BEE_NUM_BASE_DEC) {
            *error = BEE_TOKEN_ERROR_UNSUPPORTED_NUM_BASE;
            return 0;
        }

        i++;
        *is_float = true;
        *type = BEE_NUM_TYPE_F32;
    }

    for (; i < sl; i++) {
        if (!is_digit(*(s + i), *base)) {
            break;
        }
    }

    static char *suffixes[] = {
        [BEE_NUM_TYPE_I8] =  "_i8",
        [BEE_NUM_TYPE_I16] = "_i16",
        [BEE_NUM_TYPE_I32] = "_i32",
        [BEE_NUM_TYPE_I64] = "_i64",
        [BEE_NUM_TYPE_U8] =  "_u8",
        [BEE_NUM_TYPE_U16] = "_u16",
        [BEE_NUM_TYPE_U32] = "_u32",
        [BEE_NUM_TYPE_U64] = "_u64",
        [BEE_NUM_TYPE_F32] = "_f32",
        [BEE_NUM_TYPE_F64] = "_f64",
        NULL,
    };
    for (size_t ss = 0L; suffixes[ss] != NULL; ss++) {
        if (jit_strcmp(suffixes[ss], s+i) == 0) {
            *type = ss;
            i += jit_strlen(suffixes[ss]);
        }
    }

    return i;
}

static size_t try_read_lit_str(const char *s, enum bee_token_error *error) {
    size_t i;
    if (*s != '"') {
        return 0;
    }

    size_t sl = jit_strlen(s);
    for (i = 1; i < sl; i++) {
        if (*(s + i) == '\\' && i + 1 < sl) {
            if (*(s + i + 1) == '"') {
                i +=1 ;
                continue;
            }
        }

        if (*(s + i) == '"') {
            break;
        }
    }

    if (*(s + i) != '"') {
        *error = BEE_TOKEN_ERROR_UNTERMINATED_STRING;
    }

    // include both '"' (starting and ending)
    return i + 1;
}

static size_t is_string_keyword(const char *s, const size_t len,
                                enum bee_keyword_type *keyword_type) {
    for (size_t k = 1; k < BEE_KEYWORD_TYPES; k++) {
        const char *keyword = keywords_table[k];
        if (len == jit_strlen(keyword) && jit_strncmp(keyword, s, len) == 0) {
            *keyword_type = k;
            return len;
        }
    }

    *keyword_type = 0;
    return 0;
}

static size_t try_read_punct(const char *s, enum bee_punct_type *punct_type, enum bee_token_error *error) {
    if (!is_punct(*s)) {
        *punct_type = 0;
        return 0;
    }

    size_t s_len = jit_strlen(s);
    for (size_t p = 1; p < BEE_PUNCT_TYPES; p++) {
        const char *cmp_punct = puncts_table[p];
        const size_t cmp_len = jit_strlen(cmp_punct);
        if (s_len < cmp_len) {
            continue;
        }

        if (jit_strncmp(cmp_punct, s, cmp_len) == 0) {
            *punct_type = p;
            return cmp_len;
        }
    }

    *punct_type = 0;
    *error = BEE_TOKEN_ERROR_UNKNOWN_PUNCT;
    return 0;
}

struct bee_token bee_token_start(const char *name, const char *data) {
    return (struct bee_token) {
            .name = name,
            .data = data,
            .off = 0L,
            .len = 0L,
            .row = 1L,
            .col = 0L,
            .type = BEE_TOKEN_TYPE_NONE,
            .error = BEE_TOKEN_ERROR_NONE,
            .keyword_type = BEE_KEYWORD_NONE,
            .punct_type = BEE_PUNCT_NONE,
    };
}

struct bee_token bee_token_next(struct bee_token prev) {
    struct bee_token cur = {
            .name = prev.name,
            .data = prev.data,
            .off = prev.off + prev.len,
            .len = 0L,
            .row = prev.row,
            .col = prev.col + prev.len,
            .type = BEE_TOKEN_TYPE_EOF,
            .keyword_type = BEE_KEYWORD_NONE,
            .punct_type = BEE_PUNCT_NONE,
            .error = BEE_TOKEN_ERROR_NONE,
    };

    // Return end of file
    if (is_eof(*(cur.data + cur.off))) {
        return cur;
    }

    // Skip spaces
    size_t read_len = try_read_spaces(cur.data + cur.off);
    cur.off += read_len;
    cur.col += read_len;

    // Try read end of line
    read_len = try_read_eol(cur.data + cur.off);
    if (read_len > 0) {
        cur.type = BEE_TOKEN_TYPE_EOL;
        cur.len = read_len;
        cur.row += read_len;
        cur.col = 0L;
        return cur;
    }

    // Try read number
    bool is_float = false;
    enum bee_num_base base;
    enum bee_num_type type;
    enum bee_token_error error;
    read_len = try_read_lit_num(cur.data + cur.off, &is_float, &base, &type, &error);
    if (read_len > 0) {
        cur.type = BEE_TOKEN_TYPE_NUMBER;
        cur.len = read_len;
        cur.num_base = base;
        cur.num_type = type;
        return cur;
    }

    // Try read string
    read_len = try_read_lit_str(cur.data + cur.off, &cur.error);
    if (read_len > 0) {
        cur.type = BEE_TOKEN_TYPE_STRING;
        cur.len = read_len;
        return cur;
    }

    // Try read punct
    read_len = try_read_punct(cur.data + cur.off, &cur.punct_type, &cur.error);
    if (read_len > 0) {
        cur.type = BEE_TOKEN_TYPE_PUNCT;
        cur.len = read_len;
        return cur;
    }

    // Read an id, it's a keyword or boolean if it matches any of the reserved
    // strings
    read_len = try_read_id(cur.data + cur.off);
    if (is_string_keyword(cur.data + cur.off, read_len, &cur.keyword_type)) {
        cur.type = (cur.keyword_type == BEE_KEYWORD_TRUE ||
                    cur.keyword_type == BEE_KEYWORD_FALSE)
                   ? BEE_TOKEN_TYPE_BOOLEAN
                   : BEE_TOKEN_TYPE_KEYWORD;
        cur.len = read_len;
        return cur;
    }

    if (read_len > 0) {
        cur.type = BEE_TOKEN_TYPE_ID;
        cur.len = read_len;
        return cur;
    }

    cur.len = 1L;
    cur.error = BEE_TOKEN_ERROR_UNKNOWN_CHAR;
    return cur;
}
