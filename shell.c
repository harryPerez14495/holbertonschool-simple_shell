#include "shell.h"

/**
 * main - Entry point for the simple shell
 * @argc: Number of arguments
 * @argv: Array of arguments
 * @env: Environment variables
 *
 * Return: Always 0.
 */
int main(int argc, char **argv, char **env)
{
	char *line = NULL;
	char *args[64];
	char *token;
	size_t len = 0;
	ssize_t nread;
	pid_t pid;
	int i;

	(void)argc;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "($) ", 4);

		nread = getline(&line, &len, stdin);

		if (nread == -1)
		{
			free(line);
			return (0);
		}

		if (nread > 0 && line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		i = 0;
		token = strtok(line, " \t");

		while (token != NULL && i < 63)
		{
			args[i] = token;
			i++;
			token = strtok(NULL, " \t");
		}

		args[i] = NULL;

		if (args[0] == NULL)
			continue;

		pid = fork();

		if (pid == -1)
		{
			perror("fork");
			free(line);
			return (1);
		}

		if (pid == 0)
		{
			execve(args[0], args, env);
			perror(argv[0]);
			exit(127);
		}

		wait(NULL);
	}

	return (0);
}
