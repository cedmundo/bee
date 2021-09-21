#include <jit/jit-common.h>
#include <jit/jit-insn.h>
#include <jit/jit-type.h>
#include <jit/jit-util.h>
#include <jit/jit-value.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <jit/jit.h>
#include <stdbool.h>
#include <errno.h>

enum ast_node_type {
  AST_NODE_TYPE_I32,
  AST_NODE_TYPE_ADD,
  AST_NODE_TYPE_SUB,
  AST_NODE_TYPE_MUL,
  AST_NODE_TYPE_DIV,
  AST_NODE_TYPE_MOD,
  AST_NODE_TYPE_BIT_AND,
  AST_NODE_TYPE_BIT_OR,
  AST_NODE_TYPE_BIT_XOR,
  AST_NODE_TYPE_BIT_LSHIFT,
  AST_NODE_TYPE_BIT_RSHIFT,
  AST_NODE_TYPE_EQ,
  AST_NODE_TYPE_NEQ,
  AST_NODE_TYPE_LT,
  AST_NODE_TYPE_LTE,
  AST_NODE_TYPE_GT,
  AST_NODE_TYPE_GTE,
  AST_NODE_TYPE_LOG_AND,
  AST_NODE_TYPE_LOG_OR,
};

struct ast_node {
  struct ast_node *left;
  struct ast_node *right;
  int32_t as_i32;
  enum ast_node_type type;
};

struct ast_node *ast_node_new() {
  return (struct ast_node *) jit_calloc(sizeof(struct ast_node), 1);
}

struct ast_node *ast_node_new_binary(enum ast_node_type type, struct ast_node *left, struct ast_node *right) {
  struct ast_node *node = ast_node_new();
  node->type = type;
  node->left = left;
  node->right = right;
  return node;
}

void ast_node_free(struct ast_node *node) {
  assert(node != NULL);
  if (node->left != NULL) {
    ast_node_free(node->left);
  }

  if (node->right != NULL) {
    ast_node_free(node->right);
  }

  free(node);
}

void ast_node_print(struct ast_node *node, size_t depth);
void ast_node_print(struct ast_node *node, size_t depth) {
  for(size_t i=0;i<depth;i++) printf(" ");

  if (node == NULL) {
    return;
  }

  static const char *node_type_strings[] = {
    "I32",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "BIT AND",
    "BIT OR",
    "BIT XOR",
    "BIT LSHIFT",
    "BIT RSHIFT",
    "EQ",
    "NEQ",
    "LT",
    "LTE",
    "GT",
    "GTE",
    "LOGICAL AND",
    "LOGICAL OR",
  };

  if (node->type == AST_NODE_TYPE_I32) {
    printf("[I32 %d]\n", node->as_i32);
  } else {
    printf("[%s]\n", node_type_strings[node->type]);
  }

  if (node->left != NULL) {
    ast_node_print(node->left, depth+1);
  }

  if (node->right != NULL) {
    ast_node_print(node->right, depth+1);
  }
}

bool match(const char **rest, const char *seq) {
  size_t seq_len = jit_strlen(seq);
  if (jit_memcmp(seq, *rest, seq_len) == 0) {
    *rest += seq_len;
    return true;
  }

  return false;
}

struct ast_node *parse_expr(const char *cur, const char **rest);
struct ast_node *parse_logical_expr(const char *cur, const char **rest);
struct ast_node *parse_comparision_expr(const char *cur, const char **rest);
struct ast_node *parse_inequality_expr(const char *cur, const char **rest);
struct ast_node *parse_bitwise_expr(const char *cur, const char **rest);
struct ast_node *parse_bitshift_expr(const char *cur, const char **rest);
struct ast_node *parse_factor_expr(const char *cur, const char **rest);
struct ast_node *parse_term_expr(const char *cur, const char **rest);
struct ast_node *parse_primary_expr(const char *cur, const char **rest);
struct ast_node *parse_number_lit(const char **rest);
bool parse_digit(const char **rest);
bool parse_space(const char **rest);
size_t parse_spaces(const char **rest);

struct ast_node *parse_expr(const char *cur, const char **rest) {
  return parse_logical_expr(cur, rest);
}

struct ast_node *parse_logical_expr(const char *cur, const char **rest) {
  struct ast_node *node = parse_comparision_expr(cur, rest);
  for(;;) {
    parse_spaces(rest);

    if (match(rest, "||")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_LOG_OR,
          node,
          parse_comparision_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, "&&")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_LOG_AND,
          node,
          parse_comparision_expr(cur, rest)
      );
      continue;
    }

    return node;
  }
}

struct ast_node *parse_comparision_expr(const char *cur, const char **rest) {
  struct ast_node *node = parse_inequality_expr(cur, rest);
  for(;;) {
    parse_spaces(rest);

    if (match(rest, "==")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_EQ,
          node,
          parse_inequality_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, "!=")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_NEQ,
          node,
          parse_inequality_expr(cur, rest)
      );
      continue;
    }

    return node;
  }
}

struct ast_node *parse_inequality_expr(const char *cur, const char **rest) {
  struct ast_node *node = parse_bitwise_expr(cur, rest);
  for(;;) {
    parse_spaces(rest);

    if (match(rest, ">=")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_GTE,
          node,
          parse_bitwise_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, "<=")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_LTE,
          node,
          parse_bitwise_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, ">")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_GT,
          node,
          parse_bitwise_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, "<")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_LT,
          node,
          parse_bitwise_expr(cur, rest)
      );
      continue;
    }

    return node;
  }
}

struct ast_node *parse_bitwise_expr(const char *cur, const char **rest) {
  struct ast_node *node = parse_bitshift_expr(cur, rest);
  for(;;) {
    parse_spaces(rest);

    if (match(rest, "& ")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_BIT_AND,
          node,
          parse_bitshift_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, "| ")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_BIT_OR,
          node,
          parse_bitshift_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, "^")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_BIT_XOR,
          node,
          parse_bitshift_expr(cur, rest)
      );
      continue;
    }


    return node;
  }
}

struct ast_node *parse_bitshift_expr(const char *cur, const char **rest) {
  struct ast_node *node = parse_factor_expr(cur, rest);
  for(;;) {
    parse_spaces(rest);

    if (match(rest, "<<")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_BIT_LSHIFT,
          node,
          parse_factor_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, ">>")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_BIT_RSHIFT,
          node,
          parse_factor_expr(cur, rest)
      );
      continue;
    }

    return node;
  }
}

struct ast_node *parse_factor_expr(const char *cur, const char **rest) {
  struct ast_node *node = parse_term_expr(cur, rest);
  for(;;) {
    parse_spaces(rest);

    if (match(rest, "-")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_SUB,
          node,
          parse_term_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, "+")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_ADD,
          node,
          parse_term_expr(cur, rest)
      );
      continue;
    }

    return node;
  }
}

struct ast_node *parse_term_expr(const char *cur, const char **rest) {
  struct ast_node *node = parse_primary_expr(cur, rest);
  for(;;) {
    parse_spaces(rest);

    if (match(rest, "*")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_MUL,
          node,
          parse_term_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, "/")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_DIV,
          node,
          parse_term_expr(cur, rest)
      );
      continue;
    }

    if (match(rest, "%")) {
      node = ast_node_new_binary(
          AST_NODE_TYPE_MOD,
          node,
          parse_term_expr(cur, rest)
      );
      continue;
    }

    return node;
  }
}

struct ast_node *parse_primary_expr(const char *cur, const char **rest) {
  parse_spaces(rest);

  if (match(rest, "(")) {
    struct ast_node *node = parse_expr(cur, rest);
    if (!match(rest, ")")) {
      printf("error: expecting an ')', found: '%c'\n", **rest);
    }
    return node;
  }

  return parse_number_lit(rest);
}

struct ast_node *parse_number_lit(const char **rest) {
  const char *base_digits = *rest;
  for (;parse_digit(rest););

  // FIXME: use count_digits and a own version of atoi
  if (base_digits == *rest) {
    return NULL;
  }

  struct ast_node *node = ast_node_new();
  node->type = AST_NODE_TYPE_I32;
  node->as_i32 = 0;
  node->as_i32 = atoi(base_digits);
  return node;
}

bool parse_digit(const char **rest) {
  return (
      match(rest, "0") ||
      match(rest, "1") ||
      match(rest, "2") ||
      match(rest, "3") ||
      match(rest, "4") ||
      match(rest, "5") ||
      match(rest, "6") ||
      match(rest, "7") ||
      match(rest, "8") ||
      match(rest, "9")
  );
}

bool parse_space(const char **rest) {
  return match(rest, " ") || match(rest, "\t");
}

size_t parse_spaces(const char **rest) {
  size_t count = 0;
  for(;parse_space(rest); count++);
  return count;
}

jit_value_t compile_expr(jit_function_t f, struct ast_node *node) {
  jit_value_t value;
  switch (node->type) {
    case AST_NODE_TYPE_I32:
      value = jit_value_create_nint_constant(f, jit_type_int, node->as_i32);
      break;
    case AST_NODE_TYPE_ADD:
      value = jit_insn_add(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_SUB:
      value = jit_insn_sub(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_MUL:
      value = jit_insn_mul(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_DIV:
      value = jit_insn_div(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_MOD:
      value = jit_insn_rem(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_BIT_AND:
      value = jit_insn_and(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_BIT_OR:
      value = jit_insn_or(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_BIT_XOR:
      value = jit_insn_xor(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_BIT_LSHIFT:
      value = jit_insn_shl(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_BIT_RSHIFT:
      value = jit_insn_shr(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_EQ:
      value = jit_insn_eq(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_NEQ:
      value = jit_insn_ne(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_GT:
      value = jit_insn_gt(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_GTE:
      value = jit_insn_ge(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_LT:
      value = jit_insn_lt(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_LTE:
      value = jit_insn_le(f, compile_expr(f, node->left), compile_expr(f, node->right));
      break;
    case AST_NODE_TYPE_LOG_AND:
      {
        jit_value_t tmp = jit_value_create(f, jit_type_sys_bool);
        jit_label_t a_label = jit_label_undefined;
        jit_label_t b_label = jit_label_undefined;
        jit_insn_branch_if(f, compile_expr(f, node->left), &a_label);
        jit_insn_store(f, tmp, jit_value_create_nint_constant(f, jit_type_sys_bool, 0));
        jit_insn_branch(f, &b_label);

        jit_insn_label(f, &a_label);
        jit_insn_store(f, tmp, compile_expr(f, node->right));

        jit_insn_label(f, &b_label);
        value = jit_insn_load(f, tmp);
        break;
      }
    case AST_NODE_TYPE_LOG_OR:
      {
        jit_value_t tmp = jit_value_create(f, jit_type_sys_bool);
        jit_label_t a_label = jit_label_undefined;
        jit_label_t b_label = jit_label_undefined;
        jit_insn_branch_if_not(f, compile_expr(f, node->left), &a_label);
        jit_insn_store(f, tmp, jit_value_create_nint_constant(f, jit_type_sys_bool, 1));
        jit_insn_branch(f, &b_label);

        jit_insn_label(f, &a_label);
        jit_insn_store(f, tmp, compile_expr(f, node->right));

        jit_insn_label(f, &b_label);
        value = jit_insn_load(f, tmp);
        break;
      }
  }
  return value;
}

char *load_file(const char *path) {
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *data = jit_calloc(sizeof(char), file_size+2);
  fread(data, file_size, sizeof(char), file);
  fclose(file);
  return data;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: bee <file.bee>\n");
    return 1;
  }

  char *src = load_file(argv[1]);
  if (src == NULL) {
    printf("could not open source file!\n");
    return errno;
  }

  const char *cpy = src;
  struct ast_node *node = parse_expr(src, &cpy);
  if (node == NULL) {
    node = ast_node_new();
    node->type = AST_NODE_TYPE_I32;
    node->as_i32 = 0;
  }

  // printf("=== source ===\n%s\n", src);
  // ast_node_print(node, 0);
  // printf("\n");

  jit_context_t context = jit_context_create();

  jit_context_build_start(context);
  jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, jit_type_int, NULL, 0, 1);
  jit_function_t function = jit_function_create(context, signature);
  jit_type_free(signature);

  jit_value_t ret_value = compile_expr(function, node);
  jit_insn_return(function, ret_value);

  jit_function_compile(function);
  jit_context_build_end(context);

  // Apply function
  jit_int result;
  jit_function_apply(function, NULL, &result);

  printf("%d", (int)result);
  ast_node_free(node);
  fflush(stdout);
  return 0;
}
