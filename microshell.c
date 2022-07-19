/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfabian <cfabian@student.42wolfsburg.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 19:55:33 by cfabian           #+#    #+#             */
/*   Updated: 2022/07/19 12:33:14 by cfabian          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"
#include <stdio.h>

int g_last_exit = 0;

void fatal_check(int i)
{
	if (i == -1)
	{
		write(2, "error: fatal\n", 14);
		exit(EXIT_FAILURE);
	}
}

int ft_strlen(char *str)
{
	int i = 0;

	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

int ft_cd(char **arg)
{
	if (!arg[1] || (arg[2] && strcmp(arg[2], ";") != 0 && strcmp(arg[2], "|") != 0))
	{
		write(2, "error: cd: bad arguments\n", 26);
		return (1);
	}
	if (chdir(arg[1]) == 0)
		return (0);
	write(2, "error: cd: cannot change directory to ", 39);
	write(2, arg[1], ft_strlen(arg[1]));
	write(2, "\n", 2);
	return (1);
}

int 	find_next(char **strings, char *str)
{
	int i = 0;
	
	if (!strings)
		return (0);
	while (strings[i])
	{
		if (strcmp(strings[i], str) == 0)
			break ;
		i++;
	}
	return (i);
}

void	replace_cmd_end_with_null(char **cmd)
{
	int i = 0;
	if(!cmd)
		return ;
	while (cmd[i])
	{
		if (strcmp(cmd[i], "|") == 0 || strcmp(cmd[i], ";") == 0)
		{
			cmd[i] = NULL;
			return ;
		}
		i++;
	}	
}

void	pipe_and_exec(char **cmd, int old_pipe[2], char **envp)
{
	int pipes[2];
	int next_pipe = (find_next(cmd, "|") < find_next(cmd, ";"))? 1 : 0;
	
	if (next_pipe)
		fatal_check(pipe(pipes));
	pid_t pid = fork();
	fatal_check(pid);
	if (pid == 0)
	{
		if (old_pipe[0] != STDIN_FILENO)
		{
			fatal_check(dup2(old_pipe[READ], STDIN_FILENO));
		}
		close(old_pipe[READ]);
		close(old_pipe[WRITE]);
		if (next_pipe)
		{
			fatal_check(dup2(pipes[WRITE], STDOUT_FILENO));
			close(pipes[WRITE]);
			close(pipes[READ]);
		}
		replace_cmd_end_with_null(cmd);
		execve(cmd[0], cmd, envp);
		write(2, "error: cannot execute ", 23);
		write(2, cmd[0], ft_strlen(cmd[0]));
		write(2, "\n", 2);
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
	{
		close(old_pipe[READ]);
		close(old_pipe[WRITE]);
		waitpid (pid, &g_last_exit, WUNTRACED);
		if (next_pipe)
		{
			int offset = find_next(cmd, "|") + 1;
			close(pipes[WRITE]);
			pipe_and_exec(cmd + offset, pipes, envp);
			close(pipes[READ]);
		}
	}
}

void	ft_exec(char **argv_start, char **envp)
{
	int 	oldpipe[2];
	
	if (!argv_start || !argv_start[0] || strcmp(argv_start[0], ";") == 0)
		return ;
	if (strcmp(argv_start[0], "cd") == 0)
		ft_cd(argv_start);
	else
	{
		oldpipe[0] = STDIN_FILENO;
		pipe_and_exec(argv_start, oldpipe, envp);
	}
}

int	main(int argc, char **argv, char **envp)
{
	int 		i = 1;

	while (i < argc)
	{
		ft_exec(argv + i, envp);
		i += find_next(argv + i, ";") + 1;
	}
	return (g_last_exit);
}
