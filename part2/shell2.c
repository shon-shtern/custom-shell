#include "shell2.h"


int main(void)
{

	char *error_prefix = "error: ";
	struct sigaction action = {0};

	action.sa_handler = SIG_IGN;
	action.sa_flags = 0;

	sigemptyset(&action.sa_mask);

	if (syscall(SYS_rt_sigaction, SIGINT, &action, NULL, 8) == -1) {
		write(2, error_prefix, strlen(error_prefix));
		write(2, strerror(errno), strlen(strerror(errno)));
		write(2, "\n", 1);

	}
	while (1) {
		char *in_comm = NULL;
		size_t in_len = 0;

		/*
		 * Need to remove newline after reading.
		 * I switch it to whitespace for tokenization
		 * so that the last command is also included.
		 */
		write(1, "$", 1);
		size_t line = getline2(&in_comm, &in_len);

		if (line == -1) {

			write(2, error_prefix, strlen(error_prefix));
			write(2, strerror(errno), strlen(strerror(errno)));
			write(2, "\n", 1);

			continue;
		}

		if (strlen(in_comm) == 1) {

			if (munmap(in_comm, strlen(in_comm) + 1) == -1) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);
				return 1;
			}
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
			if (munmap(in_comm, strlen(in_comm) + 1) == -1) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);
				return 1;
			}
			continue;
		}

		char *args[num_tokens + 1];
		int flags = PROT_READ|PROT_WRITE;
		int flags2 = MAP_ANONYMOUS|MAP_PRIVATE;

		args[0] = mmap(NULL, strlen(command) + 1, flags, flags2, -1, 0);

		if (args[0] == MAP_FAILED) {
			size_t error_call = strlen(strerror(errno));

			write(2, error_prefix, strlen(error_prefix));
			write(2, strerror(errno), error_call);
			write(2, "\n", 1);

			if (munmap(in_comm, strlen(in_comm) + 1) == -1) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);

				return 1;
			}
			continue;
		}
		strcpy(args[0], command);
		int num_args = 1;
		char *arg;

		while ((arg = strtok(NULL, " "))) {
			int len = strlen(command) + 1;

			args[num_args] = mmap(NULL, len, flags, flags2, -1, 0);

			if (args[num_args] == MAP_FAILED) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);

				free_data(args, num_args);
				int unmap_len = strlen(in_comm) + 1;
				int unmap = munmap(in_comm, unmap_len);

				if (unmap == -1) {
					char *err = strerror(errno);
					size_t error_call = strlen(err);
					int err_len = strlen(error_prefix);

					write(2, error_prefix, err_len);
					write(2, strerror(errno), error_call);
					write(2, "\n", 1);
					return 1;
				}

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
			if (munmap(in_comm, strlen(in_comm) + 1) == -1) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);
				return 1;
			}
			break;
		} else if (exiting_prog == 2) {
			free_data(args, 1);
			if (munmap(in_comm, strlen(in_comm) + 1) == -1) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);
				return 1;
			}
			continue;
		}

		if (cd_check(args[0], args)) {
			free_data(args, num_args);
			if (munmap(in_comm, strlen(in_comm) + 1) == -1) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);
				return 1;
			}
			continue;
		}

		pid_t new_pid;

		int f = CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD;

		pid_t pid = syscall(SYS_clone, f, NULL, NULL, &new_pid, NULL);

		if (pid < 0) {
			size_t error_call = strlen(strerror(errno));

			write(2, error_prefix, strlen(error_prefix));
			write(2, strerror(errno), error_call);
			write(2, "\n", 1);
			continue;
		}

		if (pid > 0) {
			pid_t returned_pid;

			if (wait4(pid, &returned_pid, 0, NULL) != pid) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);
			}

		} else {
			struct sigaction a = {0};

			a.sa_handler = SIG_IGN;
			a.sa_flags = 0;
			a.sa_handler = SIG_DFL;
			a.sa_flags = 0;

			sigemptyset(&action.sa_mask);
			if (syscall(SYS_rt_sigaction, SIGINT, &a, NULL, 8)) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);

			}

			execve(command, args, environ);

			size_t error_call = strlen(strerror(errno));

			write(2, error_prefix, strlen(error_prefix));
			write(2, strerror(errno), error_call);
			write(2, "\n", 1);

			free_data(args, num_args);
			if (munmap(in_comm, strlen(in_comm) + 1) == -1) {
				size_t error_call = strlen(strerror(errno));

				write(2, error_prefix, strlen(error_prefix));
				write(2, strerror(errno), error_call);
				write(2, "\n", 1);
				return 1;
			};
			break;
		}


		// Release the memory
		free_data(args, num_args);
		if (munmap(in_comm, strlen(in_comm) + 1) == -1) {
			size_t error_call = strlen(strerror(errno));

			write(2, error_prefix, strlen(error_prefix));
			write(2, strerror(errno), error_call);
			write(2, "\n", 1);
			return 1;
		}
	}
}


/*
 * Free all of the data that was allocated to the heap.
 * This is to ensure no leaks happen in case we need to continue
 */
void free_data(char **args, int num_tokens)
{
	char *error_prefix = "error: ";

	for (int i = 0; i < num_tokens; i++) {

		if (munmap(args[i], strlen(args[i]) + 1) == -1) {
			write(2, error_prefix, strlen(error_prefix));
			write(2, strerror(errno), strlen(strerror(errno)));
			write(2, "\n", 1);
		}
	}
}

/*
 *This function handles the change directory case.
 * The function raises an error if the user implements incorrect cd arguments.
 */
int cd_check(char *command, char **args)
{
	char *error_prefix = "error: ";
	char *one_dir = "cd must have one argument.\n";

	int cmp = strcmp(command, "cd");

	if (cmp != 0)
		return 0;
	if (args[1] == NULL || args[2] != NULL) {
		write(2, error_prefix, strlen(error_prefix));
		write(2, one_dir, strlen(one_dir));

		return 1;
	}

	int potential_chdir = chdir(args[1]);

	if (potential_chdir == -1) {
		write(2, error_prefix, strlen(error_prefix));
		write(2, strerror(errno), strlen(strerror(errno)));
		write(2, "\n", 1);

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

	char *error_prefix = "error: ";
	char *no_args = "exit cannot have any arguments.\n";

	int is_exit = strcmp(command, "/bin/exit");
	int is_exit2 = strcmp(command, "exit");

	if ((is_exit == 0 || is_exit2 == 0) && args[1] == NULL) {
		return 1;
	} else if ((is_exit == 0 || is_exit2 == 0) && args[1] != NULL) {

		write(2, error_prefix, strlen(error_prefix));
		write(2, no_args, strlen(no_args));

		return 2;
	}

	return 0;
}

/*
 * This function is the equivalent of getline.
 * Here, I do all the syscalls myself.
 */
size_t getline2(char **buffer, size_t *n)
{

	size_t size_allocated = 3000;

	int f = PROT_READ|PROT_WRITE;
	int f2 = MAP_ANONYMOUS|MAP_PRIVATE;

	buffer[0] = (char *) mmap(NULL, size_allocated, f, f2, -1, 0);

	if (buffer[0] == MAP_FAILED)
		return -1;

	int i = 0;

	while (1) {

		size_t read_bytes = read(0, (buffer[0] + i), 1);

		if (read_bytes == -1) {
			munmap(buffer[0], size_allocated);
			return -1;
		} else if (*(buffer[0] + i) == '\n') {
			i++;
			break;
		}

		i++;

		// reached max value for the buffer. Allocate memory
		if (i == size_allocated) {
			int new_size = size_allocated * 2;

			char *new_buffer = mmap(NULL, new_size, f, f2, -1, 0);

			if (new_buffer == MAP_FAILED) {
				munmap(buffer[0], size_allocated);
				return -1;
			}

			for (int j = 0; j < size_allocated; j++)
				new_buffer[j] = buffer[0][j];

			if (munmap(buffer[0], size_allocated) == -1)
				return -1;

			buffer[0] = new_buffer;
			size_allocated *= 2;
		}
	}
	buffer[0][i] = '\0';
	*n = size_allocated;
	return i;
}
