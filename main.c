#include "constants.h"
#include "shell.h"

int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  ShLoop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}