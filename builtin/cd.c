#include "../io.h"

#include "cd.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int Cd(char** args) {
  if (!args[1]) {
    PrintError("missing arguments\n");
    return 1;
  }
  int ret = 0;
  if (ret = chdir(args[1])) {
    perror("cd");
  }
  return ret;
}