#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sched.h>
#include <limits.h>
#include <stddef.h>

int num_spaces(char *str);
int exiting(char *command, char **args);
int cd_check(char *command, char **args);
void exit_handler(int signal);
void free_data(char **args, int num_tokens);
size_t getline2(char **buffer, size_t *n);
