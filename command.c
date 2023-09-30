#include "command.h"
#include "exec.h"
#include "stream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void argv_to_cmds(cmdtemp_t *tmp);
static void cmdtemp_init(cmdtemp_t **tmp, char **argv, int argc, int *bg_count);

static int exec_command(cmdtemp_t *tmp);
static void tmp_to_cmd(cmdtemp_t *tmp, command_t *cmd);

static void handle_arg(cmdtemp_t *tmp);
static void array_from_to(char **from, char ***to, int start, int end);
static void skip(cmdtemp_t *tmp);
static void free_cmd(command_t *cmd);

void handle_commands(char **argv, int argc, int *bg_count)
{
	cmdtemp_t *tmp = malloc(sizeof(cmdtemp_t));
	cmdtemp_init(&tmp, argv, argc, bg_count);
	argv_to_cmds(tmp);
	free(tmp);
}

static void cmdtemp_init(cmdtemp_t **tmp, char **argv, int argc, int *bg_count)
{
	(*tmp)->argv = argv;
	(*tmp)->argc = argc;
	(*tmp)->start = 0;
	(*tmp)->end = 0;
	(*tmp)->cur = 0;
	(*tmp)->count = 0;
	(*tmp)->err_flag = 0;
	(*tmp)->bg_count = bg_count;
	(*tmp)->fd_in = 0;
	(*tmp)->fd_out = 1;
}

static void argv_to_cmds(cmdtemp_t *tmp)
{
	while(tmp->cur < tmp->argc){
		if(!tmp->err_flag)
			handle_arg(tmp);
		else
			skip(tmp);

		tmp->cur++;
	}

	exec_command(tmp);
}

static void handle_arg(cmdtemp_t *tmp)
{
	int res;
	if(tmp->start == tmp->end)
	{
		if(!strcmp(tmp->argv[tmp->cur], "cd"))
		{
			tmp->type = cd;
		}
		else
			tmp->type = usual;
	}

	else if(!strcmp(tmp->argv[tmp->cur], "&&"))
	{
		exec_command(tmp);
	}
	else if(!strcmp(tmp->argv[tmp->cur], "&"))
	{
		if(tmp->type != cd)
			tmp->type = bg;

		int ok = exec_command(tmp);
		if(!ok)
			(*tmp->bg_count)++;

	}
	else if(!strcmp(tmp->argv[tmp->cur], "<"))
	{
		tmp->fd_in = change_fd(tmp->argv[tmp->cur + 1], in, tmp->fd_in);
		tmp->end--;
		return;
	}
	else if(!strcmp(tmp->argv[tmp->cur], ">"))
	{
		tmp->fd_out = change_fd(tmp->argv[tmp->cur + 1], out, tmp->fd_out);
		tmp->end--;
		return;
	}
	else if(!strcmp(tmp->argv[tmp->cur], ">>"))
	{
		tmp->fd_out = change_fd(tmp->argv[tmp->cur + 1], append, tmp->fd_out);
		tmp->end--;
		return;
	}
	else if(tmp->type == cd && tmp->cur - tmp->start > 1)
	{
		printf("The cd have only one arg.\n");
		tmp->err_flag = 1;
	}
	tmp->end++;
}

static int exec_command(cmdtemp_t *tmp)
{
	command_t *cmd = malloc(sizeof(command_t));
	tmp_to_cmd(tmp, cmd);
	int ok = exec(cmd);
	tmp->start = tmp->cur + 1;
	tmp->end = tmp->cur;
	free_cmd(cmd);
	return ok;
}

static void tmp_to_cmd(cmdtemp_t *tmp, command_t *cmd)
{
	array_from_to(tmp->argv, &cmd->args, tmp->start, tmp->end);
	cmd->type = tmp->type;
	cmd->fd_in = tmp->fd_in;
	cmd->fd_out = tmp->fd_out;
}

static void array_from_to(char **from, char ***to, int start, int end)
{
	int size = end - start;
	*to = malloc((size + 1) * sizeof(char*));

	for(int i = 0; i < size; i++)
		(*to)[i] = from[start + i];
	
	(*to)[size] = NULL;
}

static void skip(cmdtemp_t *tmp)
{
	if(!strcmp(tmp->argv[tmp->end], "&&")	|| 
		!strcmp(tmp->argv[tmp->end], "&")	|| 
		tmp->end == tmp->argc)
	{
		tmp->err_flag = 0;
	}
}

static void free_cmd(command_t *cmd)
{
	free(cmd->args);
	free(cmd);
}

