#ifndef COMMAND_H
#define COMMAND_H

#include "parse.h"

typedef enum cmdtype
{
	usual,
	bg,
	cd,
	pip,
}
cmdtype_t;

typedef struct command
{
	char **argv;
	cmdtype_t type;
	int fd[2];
	int pipe_in_tmp;
	int pgid;
}
command_t;

typedef enum cmd_err
{
	no_cmd_err,
	cd_many_args_err,
	cd_err,
	exec_err,
	fork_err,
	fopen_err,
	usr_err,
}
cmd_err_t;

typedef struct cmdtemp
{
	command_t *cmd;
	argument_t *argv;
	int argc;
	int start;
	int end;
	int cur;
	int count;
	int prev_rdpipe;
	cmd_err_t err;
}
cmdtemp_t;

void handle_commands(argument_t *argv, int argc);

#endif
