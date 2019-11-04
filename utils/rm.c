#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
int rm(const char* path) {
  int status = unlink(path);
  if (status)
    fprintf(stderr, "%s: %s\n", path, strerror(errno));
  return status;
}
int main(int argc, char** argv) {
  puts("*rsh version*");
  if (argc > 1) {
    int status = 0;
    for (int i = 1; i < argc; i++)
      status |= rm(argv[i]);
    return status;
  } else {
    fprintf(stderr, "rm: missing operand\nTry 'rm --help' for more information.\n");
    return 1;
  }
}