#ifndef COMMAND_H
#define COMMAND_H

#include "parse.h"

typedef enum cmdtype
{
	usual,
	bg,
	cd,
	pip
}
cmdtype_t;

typedef struct command
{
	/* array for execvp */
	char **argv;
	cmdtype_t type;

	/* input and output of command */
	int fd[2];

	/* temp input fd of pipe if it has created in previous command */
	int pipe_in_tmp;

	/* first proc id that which is leader of group */
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
	usr_err
}
cmd_err_t;

typedef struct cmdtemp
{
	command_t *cmd;

	/* result of parsing */
	argument_t *argv;
	int argc;

	/* argv border for each command */
	int start;
	int end;
	
	/* index of handling argument */
	int cur;

	/* temp input fd of pipe if it has created in previous command */
	int prev_rdpipe;

	cmd_err_t err;
}
cmdtemp_t;

void handle_commands(argument_t *argv, int argc);

#endif
