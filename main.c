#include <stdio.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: bee <file.bee>\n");
    return 1;
  }

  printf("0\n");
  return 0;
}
