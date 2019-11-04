#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <errno.h>

extern int optind;
bool soft_link;
bool IsDir(char* path) {
  struct stat statbuf;
  stat(path, &statbuf);
  if (errno) return false;
  return (statbuf.st_mode & __S_IFMT) == __S_IFDIR;
}
int ln(char* src, char* dest) {
  char new_dest[1024];
  strcpy(new_dest, dest);
  if (IsDir(dest)) {
    int n = strlen(new_dest);
    if (new_dest[n-1] != '/') {
      new_dest[n++] = '/';
      new_dest[n] = 0;
    }
    strcat(new_dest, basename(src));
  }
  int status = 0;
  errno = 0;
  if (soft_link)
    status |= symlink(src, new_dest);
  else
    status |= link(src, new_dest);
  if (errno)
    perror("ln");
  return 0;
}
int main(int argc, char** argv) {
  puts("*rsh version*");
  for (int ch;(ch = getopt(argc, argv, "s")) != -1;) {
    switch (ch) {
      case 's':
        soft_link = true;
        break;
      default:
        break;
    }
  }
  if (argc - optind < 1) {
    fprintf(stderr, "ln: missing file operand\nTry 'ln --help' for more information.\n");
    return 1;
  }
  else if (argc - optind > 2) {
    if (access(argv[argc-1], F_OK) != -1 && !IsDir(argv[argc-1])) {
      fprintf(stderr, "ln: destination is a regular file\n");
      return 1;
    }
  }
  int status = 0;
  for (int i = optind; i < argc - 1; i++) {
    status |= ln(argv[i], argv[argc-1]);
  }
  return status;
}