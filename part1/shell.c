#include "shell.h"

int main(void)
{
	while (1) {
		signal(SIGINT, SIG_IGN);
		char *in_comm = NULL;
		size_t in_len = 0;

		/*
		 * Need to remove newline after reading.
		 * I switch it to whitespace for tokenization
		 * so that the last command is also included.
		 */
		printf("$");
		size_t line = getline(&in_comm, &in_len, stdin);

		if (line == -1) {
			fprintf(stderr, "error: %s\n", strerror(errno));
			continue;
		}

		if (strlen(in_comm) == 1) {
			free(in_comm);
			continue;
		}

		/*
		 * Used for handling \n and making sure each argument is taken
		 * and making sure each argument is taken into account using
		 * a whitespace marker.
		 */
		in_comm[line - 1] = ' ';
		const int num_tokens = num_spaces(in_comm);

		char *command = strtok(in_comm, " ");

		if (command == NULL) {
			free(in_comm);
			continue;
		}

		char *args[num_tokens + 1];

		args[0] = malloc(strlen(command) + 1);
		if (args[0] == NULL) {
			fprintf(stderr, "error: %s\n", strerror(errno));
			free(in_comm);
			continue;
		}
		strcpy(args[0], command);
		int num_args = 1;
		char *arg;

		while ((arg = strtok(NULL, " "))) {
			args[num_args] = malloc(strlen(arg) + 1);
			if (args[num_args] == NULL) {
				fprintf(stderr, "error: %s\n", strerror(errno));
				free_data(args, num_args);
				free(in_comm);
				continue;
			}
			strcpy(args[num_args], arg);
			num_args += 1;
		}

		args[num_args] = NULL;
		num_args -= 1;

		int exiting_prog = exiting(command, args);

		if (exiting_prog == 1) {
			free_data(args, 1);
			free(in_comm);
			break;
		} else if (exiting_prog == 2) {
			free_data(args, 1);
			free(in_comm);
			continue;
		}

		if (cd_check(args[0], args)) {
			free_data(args, num_args);
			free(in_comm);
			continue;
		}


		pid_t pid = fork();

		if (pid < 0) {
			fprintf(stderr, "error: %s\n", strerror(errno));
			continue;
		}

		if (pid > 0) {
			pid_t returned_pid;

			if (waitpid(pid, &returned_pid, 0) != pid)
				fprintf(stderr, "error: %s\n", strerror(errno));
		} else {
			signal(SIGINT, SIG_DFL);

			execvp(command, args);

			fprintf(stderr, "error: %s\n", strerror(errno));
			free_data(args, num_args);
			free(in_comm);
			break;
		}


		// Release the memory
		free_data(args, num_args);
		free(in_comm);
	}
}


/*
 * Free all of the data that was allocated to the heap.
 * This is to ensure no leaks happen in case we need to continue
 */
void free_data(char **args, int num_tokens)
{
	for (int i = 0; i <= num_tokens; i++)
		free(args[i]);
}

/*
 *This function handles the change directory case.
 * The function raises an error if the user implements incorrect cd arguments.
 */
int cd_check(char *command, char **args)
{
	int cmp = strcmp(command, "cd");

	if (cmp != 0)
		return 0;
	if (args[1] == NULL || args[2] != NULL) {
		fprintf(stderr, "error: %s\n", "cd must have one argument.");
		return 1;
	}

	int potential_chdir = chdir(args[1]);

	if (potential_chdir == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		return 1;
	}
	return 2;

}

/*
 *This function counts the number of spaces within a given string.
 *This is used to know how many commands are within a user input.
 */
int num_spaces(char *str)
{

	int spaces = 0;

	for (int i = 0; i < strlen(str); i++) {
		if (str[i] == 32)
			spaces += 1;
	}
	return spaces;
}

/*
 * This function determines if the argument is an exit.
 * It returns an indicator that an exit was called.
 */
int exiting(char *command, char **args)
{

	int is_exit = strcmp(command, "/bin/exit");
	int is_exit2 = strcmp(command, "exit");

	if ((is_exit == 0 || is_exit2 == 0) && args[1] == NULL) {
		return 1;
	} else if ((is_exit == 0  || is_exit2 == 0) && args[1] != NULL) {
		fprintf(stderr, "error: %s\n", "exit cannot have arguments.");
		return 2;
	}
	return 0;
}
