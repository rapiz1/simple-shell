#include "../constants.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
char** split(char* line, char* delim) {
  int bufsize = SHELL_BUFFER_LEN, position = 0;
  char** tokens = malloc(bufsize * sizeof(char*));
  char* token;

  if (!tokens) {
    fprintf(stderr, "rsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, delim);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += SHELL_BUFFER_LEN;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, SHELL_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
bool AppendArgs(char** begin, char delim) {
  if (*begin != NULL) {
    free(*begin);
    *begin = NULL;
  }
  int ch, pos = 0, cap = SHELL_BUFFER_LEN;
  char* buf = malloc(sizeof(SHELL_BUFFER_LEN));
  while ((ch = getchar()) != delim && ch != EOF) {
    if (pos == cap) {
      cap += SHELL_BUFFER_LEN;
      buf = realloc(buf, cap);
    }
    buf[pos++] = ch;
  }
  buf[pos] = 0;
  *begin = buf;
  return pos;
}
int Launch(char* path, char** args) {
  int status = EXIT_SUCCESS;
  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    status = EXIT_SUCCESS;
    if (execvp(path, args) == -1) {
      perror("xargs");
      status = EXIT_FAILURE;
    }
    exit(status);
  } else if (pid < 0) {
    // Error forking
  } else {
    // Parent process
    pid_t wpid;
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return status;
}
int main(int argc, char** argv) {
  //puts("*rsh version*");
  int status = 0;
  char delim = '\n';
  int prog_pos = 1;
  if (argc > 1) {
    if (!strcmp(argv[1], "-d")) {
      if (!argv[2]) {
        fprintf(stderr, "missing delim\n");
        exit(EXIT_FAILURE);
      } else {
        if (!strcmp(argv[2], "\n"))
          delim = '\n';
        else if (!strcmp(argv[2], "\t"))
          delim = '\t';
        else if (strlen(argv[2]) != 1) {
          fprintf(stderr, "incorrect delim\n");
          exit(EXIT_FAILURE);
        } else
          delim = argv[2][0];
      }
      prog_pos = 3;
    }
  }

  {
    char** new_argv = malloc(sizeof(char**)*(argc+3));
    memcpy(new_argv, argv, sizeof(char*)*argc);
    new_argv[argc] = new_argv[argc+1] = 0;
    argv = new_argv;
  }

  if (!argv[prog_pos]) {
    argv[argc++] = "echo";
    argv[argc] = argv[argc+1] = 0;
  }

  while (AppendArgs(argv + argc, delim))
    status |= Launch(argv[prog_pos], argv + prog_pos);
}