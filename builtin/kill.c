#include "../io.h"

#include "kill.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <signal.h>

int Kill(char** args) {
  if (!args[1]) {
    PrintError("kill: missing arguments\n");
    return 1;
  }
  int pid = atoi(args[1]);
  int ret = 0;
  if (ret=kill(pid, SIGTERM)) {
    perror("kill");
  }
  return ret;
}