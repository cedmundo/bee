//
// Created by carlos on 6/10/22.
//

#ifndef BEE_LEXER_H
#define BEE_LEXER_H
#include <stddef.h>

enum bee_token_type {
    BEE_TOKEN_TYPE_NONE,
    BEE_TOKEN_TYPE_EOF,
    BEE_TOKEN_TYPE_EOL,
    BEE_TOKEN_TYPE_PUNCT,
    BEE_TOKEN_TYPE_NUMBER,
    BEE_TOKEN_TYPE_STRING,
    BEE_TOKEN_TYPE_BOOLEAN,
    BEE_TOKEN_TYPE_KEYWORD,
    BEE_TOKEN_TYPE_ID,
};

enum bee_token_error {
    BEE_TOKEN_ERROR_NONE,
    BEE_TOKEN_ERROR_UNKNOWN_CHAR,
    BEE_TOKEN_ERROR_UNTERMINATED_STRING,
    BEE_TOKEN_ERROR_UNKNOWN_PUNCT,
    BEE_TOKEN_ERROR_UNSUPPORTED_NUM_BASE,
};

enum bee_keyword_type {
    BEE_KEYWORD_NONE,
    BEE_KEYWORD_MODULE,
    BEE_KEYWORD_IMPORT,
    BEE_KEYWORD_EXPORT,
    BEE_KEYWORD_EXTERN,
    BEE_KEYWORD_END,
    BEE_KEYWORD_FUNCTION,
    BEE_KEYWORD_RETURN,
    BEE_KEYWORD_ASSERT,
    BEE_KEYWORD_WHILE,
    BEE_KEYWORD_FOR,
    BEE_KEYWORD_IF,
    BEE_KEYWORD_ELIF,
    BEE_KEYWORD_ELSE,
    BEE_KEYWORD_IN,
    BEE_KEYWORD_AS,
    BEE_KEYWORD_IS,
    BEE_KEYWORD_SWITCH,
    BEE_KEYWORD_CASE,
    BEE_KEYWORD_CONTINUE,
    BEE_KEYWORD_BREAK,
    BEE_KEYWORD_FALLTHROUGH,
    BEE_KEYWORD_TYPE,
    BEE_KEYWORD_LET,
    BEE_KEYWORD_EFFECT,
    BEE_KEYWORD_HANDLER,
    BEE_KEYWORD_HANDLE,
    BEE_KEYWORD_RESUME,
    BEE_KEYWORD_ABORT,
    BEE_KEYWORD_WITH,
    BEE_KEYWORD_DO,
    BEE_KEYWORD_TRUE,
    BEE_KEYWORD_FALSE,
    BEE_KEYWORD_TYPES,
};

enum bee_punct_type {
    BEE_PUNCT_NONE,
    BEE_PUNCT_RANGE,
    BEE_PUNCT_WALRUS,
    BEE_PUNCT_EQ,
    BEE_PUNCT_NE,
    BEE_PUNCT_GE,
    BEE_PUNCT_LE,
    BEE_PUNCT_LOG_AND,
    BEE_PUNCT_LOG_OR,
    BEE_PUNCT_BIT_RSH,
    BEE_PUNCT_BIT_LSH,
    BEE_PUNCT_ADD,
    BEE_PUNCT_SUB,
    BEE_PUNCT_MUL,
    BEE_PUNCT_DIV,
    BEE_PUNCT_REM,
    BEE_PUNCT_BIT_AND,
    BEE_PUNCT_BIT_XOR,
    BEE_PUNCT_BIT_OR,
    BEE_PUNCT_AT,
    BEE_PUNCT_DOT,
    BEE_PUNCT_COMMA,
    BEE_PUNCT_LOG_NEG,
    BEE_PUNCT_BIT_NEG,
    BEE_PUNCT_ARI_NEG,
    BEE_PUNCT_ASSIGN,
    BEE_PUNCT_COLON,
    BEE_PUNCT_LPAR,
    BEE_PUNCT_RPAR,
    BEE_PUNCT_LSB,
    BEE_PUNCT_RSB,
    BEE_PUNCT_LCB,
    BEE_PUNCT_RCB,
    BEE_PUNCT_TYPES
};

enum bee_num_base {
    BEE_NUM_BASE_DEC,
    BEE_NUM_BASE_HEX,
    BEE_NUM_BASE_OCT,
    BEE_NUM_BASE_BIN,
};

enum bee_num_type {
    BEE_NUM_TYPE_I8,
    BEE_NUM_TYPE_I16,
    BEE_NUM_TYPE_I32,
    BEE_NUM_TYPE_I64,
    BEE_NUM_TYPE_U8,
    BEE_NUM_TYPE_U16,
    BEE_NUM_TYPE_U32,
    BEE_NUM_TYPE_U64,
    BEE_NUM_TYPE_F32,
    BEE_NUM_TYPE_F64,
};

struct bee_token {
    const char *name;
    const char *data;
    size_t off;
    size_t len;
    size_t row;
    size_t col;
    enum bee_token_type type;
    enum bee_token_error error;
    enum bee_keyword_type keyword_type;
    enum bee_punct_type punct_type;
    enum bee_num_base num_base;
    enum bee_num_type num_type;
};

struct bee_token bee_token_start(const char *name, const char *data);
struct bee_token bee_token_next(struct bee_token prev);

#endif //BEE_LEXER_H
