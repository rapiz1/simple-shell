#include <stdio.h>
int main(int argc, char** argv) {
  puts("*rsh version*");
  if (argc > 1) {
    for  (int i = 1; i < argc; i++) {
      FILE* f = fopen(argv[i], "a+");
      fclose(f);
    }
  }
  else {
    fprintf(stderr, "touch: missing file operand\nTry 'touch --help' for more information.\n");
    return 1;
  }
  return 0;
}