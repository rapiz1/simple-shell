#include "helper.h"
#include "constants.h"
#include "io.h"

#include <string.h>
#include <stdio.h>

void UnixError(char* msg) {
  if (errno) {
    char str[SHELL_BUFFER_LEN];
    sprintf(str, "%s: %s\n", msg, strerror(errno));
    PrintError(str);
  }
}
pid_t Fork() {
  pid_t pid = fork();
  if (pid < 0)
    UnixError("Fork Error");
  return pid;
}