/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfabian <cfabian@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 19:55:33 by cfabian           #+#    #+#             */
/*   Updated: 2022/06/01 20:11:59 by cfabian          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"
#include <stdio.h>

int g_last_exit = 0;

void free_data(t_data *data)
{
	if (data)
	{
		if (data->cmd)
			free(data->cmd);
		free_data(data->next);
		free(data);
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
	if (!arg[1] || arg[2])
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

char	**alloc_cmd_size(char **argv, int start)
{
	char **cmd;
	int	size = 0;
	
	while (argv[start] && strncmp(argv[start], "|", 2) &&
	strncmp(argv[start], ";", 2))
	{
		start++;
		size++;
	}
	cmd = malloc(sizeof(char *) * (size + 1));
	while (size >= 0)
	{
		cmd[size] = NULL;
		size --;
	}
	return (cmd);
}

t_data	*init_data(char **argv, int argc, int start, enum e_type before)
{
	t_data	*data = malloc(sizeof(t_data));
	data->next = NULL;
	int	i = 0;
	
	data->cmd = alloc_cmd_size(argv, start);
	data->before = before;
	data->after = END;
	while (start < argc && argv[start])
	{
		if (!strcmp(argv[start], "|"))
		{
			data->after = PIPE;
			data->next = init_data(argv, argc, start + 1, PIPE);
			break ;
		}
		else if (!strcmp(argv[start], ";"))
		{
			data->after = SEMICOL;
			data->next = init_data(argv, argc, start + 1 , START);
			break ;
		}
		else
		{
			data->cmd[i] = argv[start];
			i++;
		}
		start++;
	}
	return (data);
}

void	pipe_and_exec(t_data *data, int old_pipe[2], char **envp)
{
	int pipes[2];

	if (data->after == PIPE)
		pipe(pipes);
	pid_t pid = fork();
	if (pid == 0)
	{
		if (data->before == PIPE)
		{
			dup2(old_pipe[READ], STDIN_FILENO);
			close(old_pipe[READ]);
			close(old_pipe[WRITE]);
		}
		if (data->after == PIPE)
		{
			dup2(pipes[WRITE], STDOUT_FILENO);
			close(pipes[WRITE]);
			close(pipes[READ]);
		}
		execve(data->cmd[0], data->cmd, envp);
		write(2, "error: cannot execute ", 23);
		write(2, data->cmd[0], ft_strlen(data->cmd[0]));
		write(2, "\n", 2);
		free_data(data);
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
	{
		if (data->after == PIPE)
		{
			pipe_and_exec(data->next, pipes, envp);
			close(pipes[WRITE]);
			close(pipes[READ]);
		}
	}
}

void	ft_exec(t_data *data, char **envp)
{
	if (!data || !data->before)
		return ;
	if (data->before == PIPE || !data->cmd || !data->cmd[0])
		ft_exec(data->next, envp);
	if (data->after != PIPE && !strcmp(data->cmd[0], "cd"))
		ft_cd(data->cmd);
	else if (data->cmd)
	{
		pipe_and_exec(data, NULL, envp);
		while (waitpid (-1, 0x0, 0x0) > 0);
	}
	if (data->after == END)
		return ;
	ft_exec(data->next, envp);
}

int	main(int argc, char **argv, char **envp)
{
	if (!argv)
		return (0);
	t_data *data = (argc > 1)? init_data(argv, argc, 1, START) : NULL;
	if (data)
		ft_exec(data, envp);
	free_data(data);
	return (0);
}
