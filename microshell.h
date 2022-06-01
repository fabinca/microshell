/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfabian <cfabian@student.42wolfsburg.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 19:38:37 by cfabian           #+#    #+#             */
/*   Updated: 2022/06/01 14:13:42 by cfabian          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MICROSHELL_H
# define MICROSHELL_H

# include <unistd.h> //pipe, dup, dup2, write, execve, chdir, close, fork
# include <stdlib.h> //malloc, free, exit
# include <sys/types.h> //fork, kill, waitpid
# include <sys/wait.h> //waitpid
# include <signal.h> //signal, kill
# include <string.h> //strcmp, strncmp
# include <fcntl.h>

enum e_pipe
{
	READ,
	WRITE
};

enum e_type
{
	START,
	PIPE,
	SEMICOL,
	END	
};


typedef struct s_data
{
	char			**cmd;
	enum e_type		before;
	enum e_type		after;
	struct s_data	*next;
}	t_data;

#endif