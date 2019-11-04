#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
int main(int argc, char** argv) {
  puts("*rsh version*");
  if (argc > 1) {
    int status = 0;
    for (int i = 1; i < argc; i++) {
      mode_t mode = (1 << 9)-1;
      status |= mkdir(argv[i], mode);
      if (status)
        fprintf(stderr, "mkdir: %s\n", strerror(errno));
    }
    return status;
  }
  else {
    fprintf(stderr, "mkdir: missing operand\nTry 'mkdir --help' for more information.\n");
    return 1;
  }

}