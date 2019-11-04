#include "../io.h"
#include "../constants.h"

#include "export.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int Export(char** args) {
  int status = EXIT_SUCCESS;
  if (!args[1]) {
    for (int i = 0; __environ[i]; i++) {
      Print(__environ[i]);
      Print("\n");
    }
  }
  else {
    for (int i = 1; args[i]; i++) {
      char* p = strchr(args[i], '=');
      if (!p) status|=setenv(args[i], "", 1);
      else {
        *p = 0;
        status|=setenv(args[i], p+1, 1);
      }
    }
  }
  return status;
}