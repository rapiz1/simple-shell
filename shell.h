#ifndef SHELL_SHELL_H_
#define SHELL_SHELL_H_
#include <stdbool.h>
void ShInit();
void ShReadEnv();
void ShLoop();
char* ShReadLine();
char** ShSplitLine(char*);
int ShExecute(char**,int);
int ShLaunch(char**, bool);
#endif