#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>

extern int optind;
bool list_flag, status, print_any, multi;
// buf must have at least 10 bytes
void strmode(mode_t mode, char* buf) {
  const char chars[] = "rwxrwxrwx";
  for (size_t i = 0; i < 9; i++) {
    buf[i] = (mode & (1 << (8 - i))) ? chars[i] : '-';
  }
  buf[9] = '\0';
}
void PrintFile(struct dirent* entry, const char* path) {
  if (!list_flag) {
    printf("%s ", entry->d_name);
    return;
  }

  char file_type = '-';
  switch (entry->d_type) {
    case DT_DIR:
      file_type = 'd';
      break;
    case DT_BLK:
      file_type = 'b';
      break;
    case DT_LNK:
      file_type = 'l';
    case DT_SOCK:
      file_type = 'c';
      break;
    default:
      break;
  }
  putchar(file_type);

  struct stat buf;
  char filePath[1024];
  strcpy(filePath, path);
  int n;
  if (filePath[(n=strlen(filePath))-1] != '/') {
    filePath[n++] = '/';
    filePath[n] = 0;
  }
  strcat(filePath, entry->d_name);
  errno = 0;
  stat(filePath, &buf);
  if (errno) {
    perror("ls");
    errno = 0;
  }
  char str[100];
  strmode(buf.st_mode, str);
  printf("%s", str);

  struct passwd *upd = getpwuid(buf.st_uid);
  struct group *gpd = getgrgid(buf.st_gid);
  printf(" %3ld %s %s %8ld", buf.st_nlink, upd->pw_name, gpd->gr_name, buf.st_size);
  ctime_r(&buf.st_mtim.tv_sec, str);
  str[strlen(str)-1]=0;
  printf(" %s %s", str, entry->d_name);
  putchar('\n');
}
int ls(const char* path) {
  if (print_any)
    putchar('\n');
  if (multi)
    printf("%s:\n", path);
  DIR* dir = opendir(path);
  struct dirent* entry;
  if (!dir) {
    fprintf(stderr, "%s: %s\n", path, strerror(errno));
    return 1;
  }
  while ((entry = readdir(dir))) {
    if (entry->d_name[0] == '.') continue;
    PrintFile(entry, path);
  }
  if (!list_flag) putchar('\n');
  closedir(dir);
  return 0;
}
int main(int argc, char** argv) {
  puts("*rsh version*");
  int ch;
  while ((ch = getopt(argc, argv, "l")) != -1) {
    switch (ch) {
      case 'l':
        list_flag = true;
        break;
      default:
        fprintf(stderr,
                "invalid option -- '%c'\nTry 'ls --help' for more information.\n",
                ch);
        break;
    }
  }
  if (argc - optind > 1) multi = true;
  for (int i = optind; i < argc; i++) {
    status |= ls(argv[i]);
    print_any = true;
  }
  if (!print_any) status |= ls(".");
  return status;
}