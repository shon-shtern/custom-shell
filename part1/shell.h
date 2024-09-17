

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int num_spaces(char *str);
int exiting(char *command, char **args);
int cd_check(char *command, char **args);
void free_data(char **args, int num_tokens);
