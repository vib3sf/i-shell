#include "command.h"
#include "exec.h"
#include "stream.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void iterate_handling(cmdtemp_t *tmp);
static void cmdtemp_init(cmdtemp_t **tmp, argument_t *argv, int argc);
static void cmd_init(command_t **cmd, int pgid);

static void exec_command(cmdtemp_t *tmp);

static void handle_arg(cmdtemp_t *tmp);
static void handle_word_arg(cmdtemp_t *tmp);
static void handle_logical_arg(cmdtemp_t *tmp);
static void handle_bg_arg(cmdtemp_t *tmp);
static void handle_stream_arg(cmdtemp_t *tmp);

static char **get_exec_argv(argument_t *from, int start, int end);
static void free_cmd(command_t *cmd);

static void change_stream(cmdtemp_t *tmp, stream_t stream);

static void check_errors(cmd_err_t err);

void handle_commands(argument_t *argv, int argc)
{
	cmdtemp_t *tmp;
	cmdtemp_init(&tmp, argv, argc);
	cmd_init(&tmp->cmd, 0);

	iterate_handling(tmp);
	check_errors(tmp->err);

	tcsetpgrp(0, getpid());		/* returns controlling terminal after exec */

	free(tmp->cmd);
	free(tmp);
}

static void cmdtemp_init(cmdtemp_t **tmp, argument_t *argv, int argc)
{
	*tmp = malloc(sizeof(cmdtemp_t));
	(*tmp)->argv = argv;
	(*tmp)->argc = argc;
	(*tmp)->start = 0;
	(*tmp)->end = 0;
	(*tmp)->cur = 0;
	(*tmp)->prev_rdpipe = 0;
	(*tmp)->err = no_cmd_err;
}

static void cmd_init(command_t **cmd, int pgid)
{
	*cmd = malloc(sizeof(command_t));
	(*cmd)->fd[0] = 0;
	(*cmd)->fd[1] = 1;
	(*cmd)->type = usual;
	(*cmd)->pgid = pgid;
	(*cmd)->pipe_in_tmp = 0;
}

static void iterate_handling(cmdtemp_t *tmp)
{
	while(tmp->cur < tmp->argc)
	{
		if(tmp->err != no_cmd_err)
			return;

		handle_arg(tmp);
		tmp->cur++;
	}
}

static void handle_arg(cmdtemp_t *tmp)
{
	switch(tmp->argv[tmp->cur].type)
	{
		case word_arg:
			handle_word_arg(tmp);
			break;
		case logical_and_arg: case logical_or_arg:
			handle_logical_arg(tmp);
			break;
		case bg_arg: case pipe_arg:
			handle_bg_arg(tmp);
			break;
		case in_arg: case out_arg: case append_arg:
			handle_stream_arg(tmp);
			return;
	}
	tmp->end++;
}

static void handle_word_arg(cmdtemp_t *tmp)
{
	if(tmp->start == tmp->end && !strcmp(tmp->argv[tmp->cur].s, "cd"))
	{
		tmp->cmd->type = cd;
	}
	else if(tmp->cmd->type == cd && tmp->cur - tmp->start > 1)
	{
		tmp->err = cd_many_args_err;
	}
	else if(tmp->cur == tmp->argc - 1)
	{
		tmp->end++;
		exec_command(tmp);
	}
}

static void handle_logical_arg(cmdtemp_t *tmp)
{
	exec_command(tmp);
	if(tmp->argv[tmp->cur].type == logical_or_arg)
	{
		if(tmp->err != no_cmd_err)
		{
			check_errors(tmp->err);
			tmp->err = no_cmd_err;
		}		
		else
			tmp->err = usr_err;
	}
}

static void handle_bg_arg(cmdtemp_t *tmp)
{
	if(tmp->cmd->type == usual)
		tmp->cmd->type = (tmp->argv[tmp->cur].type == bg_arg) ? bg : pip;

	exec_command(tmp);
}

static void handle_stream_arg(cmdtemp_t *tmp)
{
	stream_t s;
	if(tmp->argv[tmp->cur].type == in_arg)
		s = in;
	else if(tmp->argv[tmp->cur].type == out_arg)
		s = out;
	else 
		s = append;

	change_stream(tmp, s);
}

static void exec_command(cmdtemp_t *tmp)
{
	int pgid_tmp;
	if(tmp->prev_rdpipe)
	{
		rd_pipe(tmp->prev_rdpipe, &tmp->cmd->fd[0]);
		tmp->prev_rdpipe = 0;
		tmp->cmd->pipe_in_tmp = 0;
	}
	if(tmp->cmd->type == pip)
	{
		crwr_pipe(&tmp->prev_rdpipe, &tmp->cmd->fd[1]);
		tmp->cmd->pipe_in_tmp = tmp->prev_rdpipe;
	}

	tmp->cmd->argv = get_exec_argv(tmp->argv, tmp->start, tmp->end);

	tmp->err = exec(tmp->cmd);

	pgid_tmp = tmp->cmd->pgid;

	tmp->start = tmp->cur + 1;
	tmp->end = tmp->cur;
	free_cmd(tmp->cmd);
	cmd_init(&tmp->cmd, pgid_tmp);
}

static char **get_exec_argv(argument_t *from, int start, int end)
{
	int size = end - start;
	char **to = malloc((size + 1) * sizeof(char*));

	int i;
	for(i = 0; i < size; i++)
		to[i] = from[start + i].s;
	
	to[size] = NULL;
	return to;
}

static void change_stream(cmdtemp_t *tmp, stream_t stream)
{
	int *fd = (stream == in) ? &tmp->cmd->fd[0] : &tmp->cmd->fd[1];

	*fd = change_fd(tmp->argv[tmp->cur + 1].s, stream, *fd);

	if(*fd == -1)
		tmp->err = fopen_err;

	tmp->end--;
}

static void check_errors(cmd_err_t err)
{
	switch(err)
	{
		case cd_many_args_err:
			printf("cd can have maximum one argument.\n");
			return;
		default:
			return;
	}
}

static void free_cmd(command_t *cmd)
{
	free(cmd->argv);
	free(cmd);
}

