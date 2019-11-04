#include <dirent.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
bool IsDir(char* path) {
  struct stat statbuf;
  stat(path, &statbuf);
  return (statbuf.st_mode & __S_IFMT) == __S_IFDIR;
}
int mv(char* src, char* dest) {
  char new_dest[1024];
  strcpy(new_dest, dest);
  if (access(dest, F_OK) != -1 && IsDir(dest)) {
    int n = strlen(new_dest);
    if (new_dest[n-1] != '/') {
      new_dest[n++] = '/';
      new_dest[n] = 0;
    }
    strcat(new_dest, basename(src));
  }
  int status = rename(src, new_dest);
  if (status)
    fprintf(stderr, "mv: %s\n", strerror(errno));
  return status;
}
int main(int argc, char** argv) {
  puts("*rsh version*");
  if (argc == 1) {
    fprintf(
        stderr,
        "mv: missing file operand\nTry 'mv --help' for more information.\n");
    return 1;
  } else {
    if (!IsDir(argv[argc - 1]) && argc > 3) {
      fprintf(stderr, "mv: destination is a regluar file\n");
      return 1;
    }
    else for (int i = 1; i < argc - 1; i++)
      mv(argv[i], argv[argc-1]);
  }
}