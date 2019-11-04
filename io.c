#include "io.h"
#include "constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE *stream[3];
void Flush() {
  fflush(stream[SHELL_STREAM_OUT]);
}
void Print(char* str) {
  fprintf(stream[SHELL_STREAM_OUT], "%s", str);
}
void PrintError(char* str) {
  fprintf(stream[SHELL_STREAM_ERR], "%s", str);
}