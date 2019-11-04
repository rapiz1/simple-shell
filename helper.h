#ifndef SHELL_HELPER_H_
#define SHELL_HELPER_H_
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
void UnixError(char* msg);
pid_t Fork();
#endif