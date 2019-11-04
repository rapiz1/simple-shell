#include "../constants.h"
#include "../io.h"

#include "echo.h"

#include <stdio.h>
#include <stdlib.h>

int Echo(char** args) {
  for (int i = 1; args[i]; i++) {
    if (i) Print(" ");
    Print(args[i]);
  }
  Print("\n");
  return EXIT_SUCCESS;
}