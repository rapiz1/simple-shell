// ATTENTION:
// only correctly handle SIGINT
// other unexpected terminated will break the setting of the terminal

#include "../constants.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>  //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>   //STDIN_FILENO

struct termios oldt, newt;
struct winsize w;
int total, lines;
void backward(int f, int num) {
  num++;
  char ch[3];
  while (num--) {
    int n;
    int pos;
    while (n = read(f, ch, 1)) {
      if (n < 0) return;
      if (!(pos = lseek(f, 0, SEEK_CUR))) return;
      if (pos < 2) {
        lseek(f, 0, SEEK_SET);
        return;
      } else
        lseek(f, -2, SEEK_CUR);
      if (ch[0] == '\n') break;
    }
  }
  lseek(f, 2, SEEK_CUR);
}
void next(int f, int num) {
  char ch[2];
  printf("              \r");
  while (num--) {
    int n;
    while (n = read(f, ch, 1)) {
      if (n < 0) break;
      putchar(ch[0]);
      if (ch[0] == '\n') break;
    }
  }
  int pos;
  if ((pos = lseek(f, 0, SEEK_CUR)) < total)
    printf("--More--(%d%%)\r", (int)((double)pos / total * 100 + 0.5));
  fflush(stdout);
}
void more(int f) {
  int len = 0;
  total = 0, lines = 0;
  char buf[SHELL_BUFFER_LEN];
  while (len = read(f, buf, SHELL_BUFFER_LEN - 1)) {
    if (len <= 0) break;
    total += len;
    for (char* p = buf; p + 1 && (p = strchr(p + 1, '\n')); lines++)
      ;
  }
  lseek(f, 0, SEEK_SET);
  next(f, w.ws_row - 1);
  while (lseek(f, 0, SEEK_CUR) < total) {
    char c = getchar();
    switch (c) {
      case 'b':
        backward(f, w.ws_row + w.ws_row - 2);
      case 'f':
        next(f, w.ws_row - 1);
        break;
      case 's':
      case '\n':
      case 'z':
      case ' ':
        next(f, 1);
        break;
      case 'q':
        return;
        break;
      default:
        break;
    }
  }
}
void restore() {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  putchar('\n');
  exit(0);
}
int main(int argc, char** argv) {
  puts("*rsh version*");

  /*tcgetattr gets the parameters of the current terminal
  STDIN_FILENO will tell tcgetattr that it should write the settings
  of stdin to oldt*/
  tcgetattr(STDIN_FILENO, &oldt);
  /*now the settings will be copied*/
  newt = oldt;

  /*ICANON normally takes care that one line at a time will be processed
  that means it will return if it sees a "\n" or an EOF or an EOL*/
  newt.c_lflag &= ~(ICANON | ECHO);

  /*Those new settings will be set to STDIN
  TCSANOW tells tcsetattr to change attributes immediately. */
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  /*This is your part:
  I choose 'e' to end input. Notice that EOF is also turned off
  in the non-canonical mode*/

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  signal(SIGINT, restore);
  if (argc == 1)
    more(STDIN_FILENO);
  else
    for (int i = 1; i < argc; i++) {
      int f = open(argv[i], O_RDONLY);
      if (f <= 0) {
        perror("more");
        continue;
      }
      if (i > 1) puts("************");
      more(f);
      close(f);
    }
  /*restore the old settings*/
  restore();
  return 0;
}