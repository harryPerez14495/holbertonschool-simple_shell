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
		full = build_path(dir, command);
		if (full != NULL)
		{
			free(path_copy);
			return (full);
		}
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

/**
 * execute_command - executes a command
 * @args: command arguments
 * @env: environment variables
 * @program: program name
 *
 * Return: 0 on success, 1 on fork failure
 */
int execute_command(char **args, char **env, char *program)
{
	char *command;
	pid_t pid;

	command = find_command(args[0]);
	if (command == NULL)
	{
		fprintf(stderr, "%s: 1: %s: not found\n",
			program, args[0]);
		return (0);
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(command);
		return (1);
	}

	if (pid == 0)
	{
		execve(command, args, env);
		perror(program);
		free(command);
		exit(127);
	}

	wait(NULL);
	free(command);
	return (0);
}

/**
 * process_line - processes one command line
 * @line: command line
 * @env: environment variables
 * @program: program name
 *
 * Return: 1 to exit shell, 0 to continue
 */
int process_line(char *line, char **env, char *program)
{
	char *args[64];

	split_line(line, args);
	if (args[0] == NULL)
		return (0);

	if (strcmp(args[0], "exit") == 0)
		return (1);

	if (strcmp(args[0], "env") == 0)
	{
		print_env(env);
		return (0);
	}

	execute_command(args, env, program);
	return (0);
}

/**
 * main - Entry point for the simple shell
 * @argc: Number of arguments
 * @argv: Array of arguments
 * @env: Environment variables
 *
 * Return: Always 0
 */
int main(int argc, char **argv, char **env)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	(void)argc;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "($) ", 4);

		nread = getline(&line, &len, stdin);
		if (nread == -1)
			break;

		if (nread > 0 && line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		if (process_line(line, env, argv[0]) == 1)
			break;
	}

	free(line);
	return (0);
}
