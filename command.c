#include "command.h"
#include "exec.h"
#include "stream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void argv_to_cmds(cmdtemp_t *tmp);
static void cmdtemp_init(cmdtemp_t **tmp, char **argv, int argc, int *bg_count);
static void cmd_init(command_t **cmd);

static void exec_command(cmdtemp_t *tmp);

static void handle_arg(cmdtemp_t *tmp);
static void array_from_to(char **from, char ***to, int start, int end);
static void free_cmd(command_t *cmd);

static void change_stream(cmdtemp_t *tmp, stream_t stream);

static void check_errors(cmd_err_t err);

void handle_commands(char **argv, int argc, int *bg_count)
{
	cmdtemp_t *tmp;
	cmdtemp_init(&tmp, argv, argc, bg_count);
	cmd_init(&tmp->cmd);

	argv_to_cmds(tmp);
	check_errors(tmp->err);

	free(tmp->cmd);
	free(tmp);
}

static void cmdtemp_init(cmdtemp_t **tmp, char **argv, int argc, int *bg_count)
{
	*tmp = malloc(sizeof(cmdtemp_t));
	(*tmp)->argv = argv;
	(*tmp)->argc = argc;
	(*tmp)->start = 0;
	(*tmp)->end = 0;
	(*tmp)->cur = 0;
	(*tmp)->count = 0;
	(*tmp)->err = no_cmd_err;
	(*tmp)->bg_count = bg_count;
}

static void cmd_init(command_t **cmd)
{
	*cmd = malloc(sizeof(command_t));
	(*cmd)->fd_in = 0;
	(*cmd)->fd_out = 1;
	(*cmd)->type = usual;
}

static void argv_to_cmds(cmdtemp_t *tmp)
{
	while(tmp->cur < tmp->argc)
	{
		if(tmp->err != no_cmd_err)
			return;

		handle_arg(tmp);
		tmp->cur++;
	}

	if(tmp->start != tmp->end && tmp->err == no_cmd_err)
		exec_command(tmp);

}

static void handle_arg(cmdtemp_t *tmp)
{
	if(tmp->start == tmp->end && !strcmp(tmp->argv[tmp->cur], "cd"))
	{
		tmp->cmd->type = cd;
	}
	else if(tmp->cmd->type == cd && tmp->cur - tmp->start > 1)
	{
		tmp->err = cd_many_args_err;
	}
	else if(!strcmp(tmp->argv[tmp->cur], "&&"))
	{
		exec_command(tmp);
	}
	else if(!strcmp(tmp->argv[tmp->cur], "&"))
	{
		if(tmp->cmd->type == usual)
			tmp->cmd->type = bg;

		exec_command(tmp);
	}
	else if(!strcmp(tmp->argv[tmp->cur], "<"))
	{
		change_stream(tmp, in);
		return;
	}
	else if(!strcmp(tmp->argv[tmp->cur], ">"))
	{
		change_stream(tmp, out);
		return;
	}
	else if(!strcmp(tmp->argv[tmp->cur], ">>"))
	{
		change_stream(tmp, append);
		return;
	}

	tmp->end++;
}

static void exec_command(cmdtemp_t *tmp)
{
	array_from_to(tmp->argv, &tmp->cmd->argv, tmp->start, tmp->end);

	tmp->err = exec(tmp->cmd);
	if(tmp->err == no_cmd_err && tmp->cmd->type == bg)
		(*tmp->bg_count)++;

	tmp->start = tmp->cur + 1;
	tmp->end = tmp->cur;
	free_cmd(tmp->cmd);
	cmd_init(&tmp->cmd);
}

static void array_from_to(char **from, char ***to, int start, int end)
{
	int size = end - start;
	*to = malloc((size + 1) * sizeof(char*));

	for(int i = 0; i < size; i++)
		(*to)[i] = from[start + i];
	
	(*to)[size] = NULL;
}

static void change_stream(cmdtemp_t *tmp, stream_t stream)
{
	int *fd = (stream == in) ? &tmp->cmd->fd_in : &tmp->cmd->fd_out;

	*fd = change_fd(tmp->argv[tmp->cur + 1], 
			stream, *fd);

	if(*fd == -1)
		tmp->err = fopen_err;

	tmp->end--;
}

static void check_errors(cmd_err_t err)
{
	switch(err)
	{
		case no_cmd_err:
			return;
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

