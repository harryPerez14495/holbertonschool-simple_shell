#include "shell.h"

/**
 * find_command - finds a command in PATH
 * @command: command to find
 *
 * Return: full path to command, or NULL
 */
char *find_command(char *command)
{
	char *path, *path_copy, *dir, *full;
	size_t size;

	if (strchr(command, '/'))
	{
		if (access(command, X_OK) == 0)
			return (strdup(command));
		return (NULL);
	}

	path = getenv("PATH");
	if (path == NULL)
		return (NULL);

	path_copy = strdup(path);
	if (path_copy == NULL)
		return (NULL);

	dir = strtok(path_copy, ":");
	while (dir != NULL)
	{
		size = strlen(dir) + strlen(command) + 2;
		full = malloc(size);
		if (full == NULL)
		{
			free(path_copy);
			return (NULL);
		}

		sprintf(full, "%s/%s", dir, command);

		if (access(full, X_OK) == 0)
		{
			free(path_copy);
			return (full);
		}

		free(full);
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

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
	char *command;
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

		if (strcmp(args[0], "exit") == 0)
		{
			free(line);
			return (0);
		}

		command = find_command(args[0]);

		if (command == NULL)
		{
			fprintf(stderr, "%s: 1: %s: not found\n",
				argv[0], args[0]);
			continue;
		}

		pid = fork();

		if (pid == -1)
		{
			perror("fork");
			free(command);
			free(line);
			return (1);
		}

		if (pid == 0)
		{
			execve(command, args, env);
			perror(argv[0]);
			free(command);
			exit(127);
		}

		wait(NULL);
		free(command);
	}

	return (0);
}
