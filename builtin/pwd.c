#include "../io.h"
#include "../constants.h"

#include "pwd.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

int Pwd(char** args) {
  Print(get_current_dir_name());
  Print("\n");
  return EXIT_SUCCESS;
}