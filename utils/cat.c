#include "../constants.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
void cat(int f) {
  char buf[SHELL_BUFFER_LEN];
  int len = 0;
  while (len = read(f, buf, SHELL_BUFFER_LEN)) {
    if (len <= 0) break;
    write(STDOUT_FILENO, buf, len);
  }
}
int main(int argc, char** argv) {
  //puts("*rsh version*");
  if (argc == 1) {
    cat(STDIN_FILENO);
  } else {
    for (int i = 1; i < argc; i++) {
      int f = open(argv[i], O_RDONLY);
      if (f > 0) {
        cat(f);
        close(f);
      } else {
        perror("cat");
      }
    }
  }
  return 0;
}