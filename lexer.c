//
// Created by carlos on 9/3/22.
//
#include "lexer.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define TRACKED_PUNCT_TYPES 27
static_assert(BEE_PUNCT_COUNT == TRACKED_PUNCT_TYPES,
              "Exhaustive punct string table");
static const char *puncts_table[] = {
        [BEE_PUNCT_NONE] = NULL,
        [BEE_PUNCT_D_GT] = ">>",
        [BEE_PUNCT_D_LT] = "<<",
        [BEE_PUNCT_D_EQ] = "==",
        [BEE_PUNCT_NE] = "!=",
        [BEE_PUNCT_GE] = ">=",
        [BEE_PUNCT_LE] = "<=",
        [BEE_PUNCT_WALRUS] = ":=",
        [BEE_PUNCT_D_PIPE] = "||",
        [BEE_PUNCT_D_AMP] = "&&",
        [BEE_PUNCT_COLON] = ":",
        [BEE_PUNCT_GT] = ">",
        [BEE_PUNCT_LT] = "<",
        [BEE_PUNCT_PLUS] = "+",
        [BEE_PUNCT_MINUS] = "-",
        [BEE_PUNCT_ASTERISK] = "*",
        [BEE_PUNCT_SLASH] = "/",
        [BEE_PUNCT_PERCENT] = "%",
        [BEE_PUNCT_AMPERSAND] = "&",
        [BEE_PUNCT_CARET] = "^",
        [BEE_PUNCT_PIPE] = "|",
        [BEE_PUNCT_TILDE] = "~",
        [BEE_PUNCT_LPAR] = "(",
        [BEE_PUNCT_RPAR] = ")",
        [BEE_PUNCT_COMMA] = ",",
        [BEE_PUNCT_BANG] = "!",
        [BEE_PUNCT_DOT] = ".",
};

#define TRACKED_KEYWORD_TYPES 4
static_assert(BEE_KEYWORD_COUNT == TRACKED_KEYWORD_TYPES,
              "Exhaustive punct string table");
static const char *keywords_table[] = {
        [BEE_KEYWORD_NONE] = NULL,
        [BEE_KEYWORD_ASSERT] = "assert",
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
    if (!is_eol(*s)) {
        return 0;
    }

    size_t i;
    for (i = 1; i < strlen(s); i++) {
        if (!is_eol(*(s + i)) && !is_space(*(s + i))) {
            break;
        }
    }

    return i;
}

static size_t count_new_lines(const char *s, size_t l) {
    size_t c = 0L;
    for (size_t i = 0; i < l; i++) {
        c += is_eol(*(s + i)) ? 1 : 0;
    }
    return c;
}

static size_t try_read_spaces(const char *s) {
    size_t i;
    for (i = 0; i < strlen(s); i++) {
        if (!is_space(*(s + i))) {
            break;
        }
    }
    return i;
}

static size_t try_read_id(const char *s) {
    size_t i;
    for (i = 0; i < strlen(s); i++) {
        if (i == 0 && !is_id_start(*s)) {
            return 0;
        }

        if (!(is_alpha(*(s + i)) || is_digit(*(s + i), BEE_NUM_BASE_DEC) || *(s + i) == '_' ||
              *(s + i) == '$')) {
            break;
        }
    }

    return i;
}

static size_t try_read_lit_num(const char *s, struct bee_loc loc, bool *is_float, enum bee_num_base *base,
                               enum bee_num_type *type, struct bee_error *error) {
    size_t i = 0;
    size_t sl = strlen(s);
    *is_float = false;
    *base = BEE_NUM_BASE_DEC;
    *type = BEE_NUM_TYPE_U32;

    if (*s == '-') {
        i += 1;
        *type = BEE_NUM_TYPE_I32;

        bool follows_digit = is_digit(*(s + 1), *base);
        bool follows_dot_with_digit = *(s + 1) == '.' && is_digit(*(s + 2), *base);
        if (!follows_digit && !follows_dot_with_digit) {
            return 0;
        }
    }

    if (strncmp("0x", s + i, 2) == 0) {
        i += 2;
        *base = BEE_NUM_BASE_HEX;
    } else if (strncmp("0b", s + i, 2) == 0) {
        i += 2;
        *base = BEE_NUM_BASE_BIN;
    } else if (strncmp("0o", s + i, 2) == 0) {
        i += 2;
        *base = BEE_NUM_BASE_OCT;
    } else if (!is_digit(*(s + i), BEE_NUM_BASE_DEC) && *(s + i) != '.') {
        return 0;
    }

    for (; i < sl; i++) {
        char digit = *(s + i);
        if (digit == '.') {
            break;
        }

        if (is_digit(digit, BEE_NUM_BASE_HEX) && !is_digit(digit, *base)) {
            bee_error_set(error, loc, "digit `%c` is outside number base", *(s + i));
            return 0;
        } else if (!is_digit(digit, BEE_NUM_BASE_HEX)) {
            break;
        }
    }

    if (*(s + i) == '.') {
        if (*base != BEE_NUM_BASE_DEC) {
            bee_error_set(error, loc, "floating point numbers can only have base 10");
            return 0;
        }

        i++;
        *is_float = true;
        *type = BEE_NUM_TYPE_F32;
    }

    for (; i < sl; i++) {
        if (memcmp(s+i, "e-", 2) == 0L || memcmp(s+i, "e+", 2) == 0L) {
            i+=2;
            continue;
        }

        char digit = *(s + i);
        if (!is_digit(digit, *base)) {
            if(*s == '.' && i == 1) {
                return 0;
            }

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
        if (strcmp(suffixes[ss], s + i) == 0) {
            *type = ss;
            i += strlen(suffixes[ss]);
        }
    }

    return i;
}

static size_t try_read_lit_str(const char *s, struct bee_loc loc, struct bee_error *error) {
    size_t i;
    if (*s != '"') {
        return 0;
    }

    size_t sl = strlen(s);
    for (i = 1; i < sl; i++) {
        if (*(s + i) == '\\' && i + 1 < sl) {
            if (*(s + i + 1) == '"') {
                i += 1;
                continue;
            }
        }

        if (*(s + i) == '"') {
            break;
        }
    }

    if (*(s + i) != '"') {
        bee_error_set(error, loc, "unterminated string");
    }

    // include both string (starting and ending)
    return i + 1;
}

static size_t is_string_keyword(const char *s, const size_t len,
                                enum bee_keyword_tag *keyword_type) {
    for (size_t k = 1; k < BEE_KEYWORD_COUNT; k++) {
        const char *keyword = keywords_table[k];
        if (len == strlen(keyword) && strncmp(keyword, s, len) == 0) {
            *keyword_type = k;
            return len;
        }
    }

    *keyword_type = 0;
    return 0;
}

static size_t
try_read_punct(const char *s, struct bee_loc loc, enum bee_punct_tag *punct_type, struct bee_error *error) {
    if (!is_punct(*s)) {
        *punct_type = 0;
        return 0;
    }

    size_t s_len = strlen(s);
    for (size_t p = 1; p < BEE_PUNCT_COUNT; p++) {
        const char *cmp_punct = puncts_table[p];
        const size_t cmp_len = strlen(cmp_punct);
        if (s_len < cmp_len) {
            continue;
        }

        if (strncmp(cmp_punct, s, cmp_len) == 0) {
            *punct_type = p;
            return cmp_len;
        }
    }

    *punct_type = 0;
    bee_error_set(error, loc, "unterminated string");
    return 0;
}

struct bee_token bee_token_start(const char *filename, const char *data) {
    return (struct bee_token) {
            .loc = {
                    .filename = filename,
                    .row = 1L,
                    .col = 0L,
            },
            .data = data,
            .len = 0L,
            .tag = BEE_TOKEN_TAG_NONE,
            .keyword_tag = BEE_KEYWORD_NONE,
            .punct_tag = BEE_PUNCT_NONE,
    };
}

struct bee_token bee_token_next(struct bee_token prev, struct bee_error *error) {
    struct bee_token cur = {
            .loc = {
                    .filename = prev.loc.filename,
                    .row =  prev.tag == BEE_TOKEN_TAG_EOL ? prev.loc.row + count_new_lines(prev.data, prev.len)
                                                          : prev.loc.row,
                    .col = prev.tag == BEE_TOKEN_TAG_EOL ? 0 : prev.len + prev.loc.col,
            },
            .data = prev.data + prev.len,
            .len = 0L,
            .tag = BEE_TOKEN_TAG_EOF,
            .keyword_tag = BEE_KEYWORD_NONE,
            .punct_tag = BEE_PUNCT_NONE,
    };

    // Return end of file
    if (is_eof(*cur.data)) {
        return cur;
    }

    // Skip spaces
    size_t read_len = try_read_spaces(cur.data);
    cur.data += read_len;
    cur.loc.col += read_len;

    // Try read end of line
    read_len = try_read_eol(cur.data);
    if (read_len > 0) {
        cur.tag = BEE_TOKEN_TAG_EOL;
        cur.len = read_len;
        return cur;
    }

    // Try read number
    bool is_float = false;
    enum bee_num_base base;
    enum bee_num_type type;
    read_len = try_read_lit_num(cur.data, cur.loc,
                                &is_float, &base, &type, error);
    if (bee_error_is_set(error)) {
        cur.tag = BEE_TOKEN_TAG_NONE;
        return cur;
    }

    if (read_len > 0) {
        cur.tag = BEE_TOKEN_TAG_NUMBER;
        cur.len = read_len;
        cur.num_base = base;
        cur.num_type = type;
        return cur;
    }

    // Try read string
    read_len = try_read_lit_str(cur.data, cur.loc, error);
    if (bee_error_is_set(error)) {
        cur.tag = BEE_TOKEN_TAG_NONE;
        return cur;
    }

    if (read_len > 0) {
        cur.tag = BEE_TOKEN_TAG_STRING;
        cur.len = read_len;
        return cur;
    }

    // Try read punct
    read_len = try_read_punct(cur.data, cur.loc, &cur.punct_tag, error);
    if (bee_error_is_set(error)) {
        cur.tag = BEE_TOKEN_TAG_NONE;
        return cur;
    }

    if (read_len > 0) {
        cur.tag = BEE_TOKEN_TAG_PUNCT;
        cur.len = read_len;
        return cur;
    }

    // Read an id, it's a keyword or boolean if it matches any of the reserved
    // strings
    read_len = try_read_id(cur.data);
    if (is_string_keyword(cur.data, read_len, &cur.keyword_tag)) {
        cur.tag = (cur.keyword_tag == BEE_KEYWORD_TRUE ||
                   cur.keyword_tag == BEE_KEYWORD_FALSE)
                  ? BEE_TOKEN_TAG_BOOLEAN
                  : BEE_TOKEN_TAG_KEYWORD;
        cur.len = read_len;
        return cur;
    }

    if (read_len > 0) {
        cur.tag = BEE_TOKEN_TAG_ID;
        cur.len = read_len;
        return cur;
    }

    cur.len = 1L;
    bee_error_set(error, cur.loc, "unknown char `%c`", *cur.data);
    return cur;
}