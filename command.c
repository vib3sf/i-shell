#include "command.h"
#include "exec.h"
#include "stream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void argv_to_cmds(cmdtemp_t *tmp);
static void cmdtemp_init(cmdtemp_t **tmp, char **argv, int argc, int *bg_count);
static void cmd_init(command_t **cmd);

static int exec_command(cmdtemp_t *tmp);

static void handle_arg(cmdtemp_t *tmp);
static void array_from_to(char **from, char ***to, int start, int end);
static void skip(cmdtemp_t *tmp);
static void free_cmd(command_t *cmd);

void handle_commands(char **argv, int argc, int *bg_count)
{
	cmdtemp_t *tmp;
	cmdtemp_init(&tmp, argv, argc, bg_count);
	cmd_init(&tmp->cmd);
	argv_to_cmds(tmp);
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
	(*tmp)->err_flag = 0;
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
	while(tmp->cur < tmp->argc){
		if(!tmp->err_flag)
			handle_arg(tmp);
		else
			skip(tmp);

		tmp->cur++;
	}

	if(tmp->start != tmp->end)
		exec_command(tmp);
}

static void handle_arg(cmdtemp_t *tmp)
{
	if(tmp->start == tmp->end && !strcmp(tmp->argv[tmp->cur], "cd"))
	{
		tmp->cmd->type = cd;
	}
	else if(!strcmp(tmp->argv[tmp->cur], "&&"))
	{
		exec_command(tmp);
	}
	else if(!strcmp(tmp->argv[tmp->cur], "&"))
	{
		if(tmp->cmd->type != cd)
			tmp->cmd->type = bg;

		int ok = exec_command(tmp);
		if(!ok)
			(*tmp->bg_count)++;

	}
	else if(!strcmp(tmp->argv[tmp->cur], "<"))
	{
		tmp->cmd->fd_in = change_fd(tmp->argv[tmp->cur + 1], 
				in, tmp->cmd->fd_in);
		tmp->end--;
		return;
	}
	else if(!strcmp(tmp->argv[tmp->cur], ">"))
	{
		tmp->cmd->fd_out = change_fd(tmp->argv[tmp->cur + 1], 
				out, tmp->cmd->fd_out);
		tmp->end--;
		return;
	}
	else if(!strcmp(tmp->argv[tmp->cur], ">>"))
	{
		tmp->cmd->fd_out = change_fd(tmp->argv[tmp->cur + 1], 
				append, tmp->cmd->fd_out);
		tmp->end--;
		return;
	}
	else if(tmp->cmd->type == cd && tmp->cur - tmp->start > 1)
	{
		printf("The cd have only one arg.\n");
		tmp->err_flag = 1;
	}
	tmp->end++;
}

static int exec_command(cmdtemp_t *tmp)
{
	array_from_to(tmp->argv, &tmp->cmd->argv, tmp->start, tmp->end);
	int ok = exec(tmp->cmd);
	tmp->start = tmp->cur + 1;
	tmp->end = tmp->cur;
	free_cmd(tmp->cmd);
	cmd_init(&tmp->cmd);
	return ok;
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
	free(cmd->argv);
	free(cmd);
}

