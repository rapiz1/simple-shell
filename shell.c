// You can not make a built-in command in background
// count 'jobs' is not implemented
#include "shell.h"
#include "constants.h"
#include "helper.h"
#include "io.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "builtin/cd.h"
#include "builtin/echo.h"
#include "builtin/export.h"
#include "builtin/kill.h"
#include "builtin/pwd.h"

extern FILE* stream[3];
void ShReadEnv() {
  if (access(SHELL_ENV_FILE, F_OK)) {
    PrintError("rsh: .rshrc not found\n");
    return;
  }
  FILE* f = fopen(SHELL_ENV_FILE, "r");
  char line[SHELL_BUFFER_LEN];
  while (fgets(line, SHELL_BUFFER_LEN, f) != NULL) {
    char** args = ShSplitLine(line);
    ShExecute(args, -1);
    free(args);
  }
  fclose(f);
}
void ShInit() {
  stream[0] = stdin;
  stream[1] = stdout;
  stream[2] = stdout;
  ShReadEnv();
  signal(SIGCHLD, SIG_IGN); // prevent zombie
}
void ShLoop() {
  char* line;
  char** args;
  int status;
  ShInit();
  signal(SIGINT, SIG_IGN);
  do {
    line = ShReadLine(stdin);
    if (!line) break;  // EOF

    args = ShSplitLine(line);
    status = ShExecute(args, -1);

    free(line);
    free(args);
  } while (true);
}
char* ShReadLine(FILE* in) {
  char* line = NULL;
  line = readline("$ ");
  return line;
}
char** ShSplitLine(char* line) {
  int bufsize = SHELL_BUFFER_LEN, position = 0;
  char** tokens = malloc(bufsize * sizeof(char*));
  char* token;

  if (!tokens) {
    fprintf(stderr, "rsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, SHELL_TOK_DELIM);
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
int ShExecute(char** args, int in) {
  if (!args || !args[0]) return EXIT_SUCCESS;  // empty line

  #ifdef DBG
  fprintf(stderr, "dbg start\n");
  for (int i = 0; args[i]; i++)
    fprintf(stderr, "%s ", args[i]);
  fprintf(stderr, "\ndbg end\n");
  #endif

  int status = 0;
  bool bg = false;
  for (int i = 0; args[i]; i++)
    if (!strcmp(args[i], "&")) {
      bg = true;
      args[i] = 0;
      for (int j = i; args[j + 1]; j++) args[j] = args[j + 1];
      break;
    }
  int saved_in = dup(0);
  int saved_out = dup(1);
  int fd[2] = {in, -1};

  int pipe_pos;
  for (pipe_pos = 0; args[pipe_pos]; pipe_pos++)
    if (!strcmp(args[pipe_pos], "|")) break;
  int pipe_fd[2] = {-1, -1};
  errno = 0;
  if (args[pipe_pos]) {
    if (pipe(pipe_fd)) perror("rsh: pipe");
    if (dup2(pipe_fd[1], 1) == -1) perror("rsh: pipe: redirect");
  }

  char** left_cmd;
  if (args[pipe_pos]) {
    args[pipe_pos] = 0;
    left_cmd = args + pipe_pos + 1;
  }
  else left_cmd = 0;

  errno = 0;
  bool valid = true;
  for (int i = 0; args[i]; i++) {
    if (!strcmp(args[i], "<")) {
      if (fd[0] != -1)
        valid = false;
      else {
        if (args[i + 1] && !access(args[i + 1], F_OK))
          fd[0] = open(args[i + 1], O_RDONLY);
        else
          valid = false;
      }
      args[i] = 0;
    } else if (!strcmp(args[i], ">")) {
      if (pipe_fd[1] != -1 || fd[1] != -1)
        valid = false;
      else {
        if (args[i + 1])
          fd[1] = open(args[i + 1], O_WRONLY|O_CREAT|O_TRUNC, 0777);
        else
          valid = false;
      }
      args[i] = 0;
    }
    if (errno) perror("rsh: redirect");
    if (!valid) break;
  }

  if (valid) {
    if (fd[0] != -1) dup2(fd[0], 0);
    if (fd[1] != -1) dup2(fd[1], 1);

    if (!strcmp(args[0], "echo"))
      status = Echo(args);
    else if (!strcmp(args[0], "exit"))
      exit(EXIT_SUCCESS);
    else if (!strcmp(args[0], "cd"))
      status = Cd(args);
    else if (!strcmp(args[0], "pwd"))
      status = Pwd(args);
    else if (!strcmp(args[0], "kill"))
      status = Kill(args);
    else if (!strcmp(args[0], "export"))
      status = Export(args);
    else {
      status = ShLaunch(args, bg);
    }
    //Flush(1);
  }


  if (pipe_fd[1] != -1) close(pipe_fd[1]);
  if (fd[0] != -1) close(fd[0]);
  if (fd[1] != -1) close(fd[1]);

  dup2(saved_in, 0);
  dup2(saved_out, 1);

  if (valid) ShExecute(left_cmd, pipe_fd[0]);
  else PrintError("rsh: invalid redirection or pipe\n");

  if (pipe_fd[0] != -1) close(pipe_fd[0]);

  return status;
}
pid_t pid, wpid;
int jobs;
void KillChild() {
  fprintf(stream[SHELL_STREAM_ERR], "Interrupt\n");
  kill(pid, SIGINT);
}
int ShLaunch(char** args, bool bg) {
  int status = EXIT_SUCCESS;

  pid = Fork();
  if (pid == 0) {
    // Child process
    status = EXIT_SUCCESS;
    if (execvp(args[0], args) == -1) {
      perror("rsh");
      status = EXIT_FAILURE;
    }
    exit(status);
  } else if (pid < 0) {
    // Error forking
  } else {
    // Parent process
    if (!bg) {
      signal(SIGINT, KillChild);
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
      signal(SIGINT, SIG_IGN);
    } else {
      jobs++;
      printf("[%d] %d ", jobs, pid);
      for (int i = 0; args[i]; i++) printf("%s ", args[i]);
      putchar('\n');
    }
  }
  return status;
}