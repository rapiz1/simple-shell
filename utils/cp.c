#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
bool IsDir(char* path) {
  struct stat statbuf;
  stat(path, &statbuf);
  return (statbuf.st_mode & __S_IFMT) == __S_IFDIR;
}
int cp(char* src, char* dest) {
  if (access(src, F_OK) == -1) {
    fprintf(stderr, "cp: %s does not exist\n", src);
    return 1;
  }
  FILE *f = fopen(src, "rb"), *fo;
  if (access(dest, F_OK) == -1) {
    fo = fopen(dest, "w");
  } else {
    char buf[1024];
    strcpy(buf, dest);
    if (IsDir(dest)) {
      int n = strlen(buf);
      if (buf[n - 1] != '/') {
        buf[n++] = '/';
        buf[n] = 0;
      }
      strcat(buf, basename(src));
    }
    fo = fopen(buf, "w");
  }
  int ch;
  while ((ch = getc(f)) != -1) putc(ch, fo);
  fclose(fo);
  fclose(f);
  return 0;
}
int main(int argc, char** argv) {
  puts("*rsh version*");
  if (argc == 1) {
    fprintf(
        stderr,
        "cp: missing file operand\nTry 'cp --help' for more information.\n");
    return 1;
  } else if (argc == 2) {
    fprintf(stderr,
            "cp: missing destination file operand after '%s'\nTry 'cp --help' "
            "for more information.\n",
            argv[1]);
    return 1;
  } else {
    int status = 0;
    if (argc > 3) {
      if (access(argv[argc - 1], F_OK) == -1) {
        fprintf(stderr, "cp: destination does not exist\n");
        return 1;
      } else if (!IsDir(argv[argc-1])) {
        fprintf(stderr, "cp: destination is a regular file\n");
        return 1;
      }
      for (int i = 1; i < argc - 1; i++) status |= cp(argv[i], argv[argc - 1]);
    } else {
      status = cp(argv[1], argv[2]);
    }
    return status;
  }
}