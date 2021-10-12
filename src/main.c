#include <bee.h>
#include <stdio.h>
#include <errno.h>
#include <jit/jit.h>

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
  struct bee_ast_node *node = bee_parse_expr(&cpy);
  if (node == NULL) {
    node = bee_ast_node_new();
    node->type = BEE_AST_NODE_TYPE_I32;
    node->as_i32 = 0;
  }

  // printf("=== source ===\n%s\n", src);
  // bee_ast_node_print(node, 0);
  // printf("\n");

  jit_context_t context = jit_context_create();

  jit_context_build_start(context);
  jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, jit_type_int, NULL, 0, 1);
  jit_function_t function = jit_function_create(context, signature);
  jit_type_free(signature);

  jit_value_t ret_value = bee_compile_expr(function, node);
  jit_insn_return(function, ret_value);

  jit_function_compile(function);
  jit_context_build_end(context);

  // Apply function
  jit_int result;
  jit_function_apply(function, NULL, &result);

  printf("%d", (int)result);
  bee_ast_node_free(node);
  fflush(stdout);
  return 0;
}
