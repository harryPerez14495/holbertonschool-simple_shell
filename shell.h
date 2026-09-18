#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/* Path functions */
char *build_path(char *dir, char *command);
char *find_command(char *command);

/* Input functions */
void split_line(char *line, char **args);
void print_env(char **env);

/* Execution functions */
int execute_command(char **args, char **env, char *program);
int process_line(char *line, char **env, char *program);

#endif
