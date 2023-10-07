#ifndef COMMAND_H
#define COMMAND_H

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
	int fd_in;
	int fd_out;
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
	char **argv;
	int argc;
	int start;
	int end;
	int cur;
	int count;
	int prev_pipe_in;
	cmd_err_t err;
}
cmdtemp_t;

void handle_commands(char **argv, int argc);

#endif
