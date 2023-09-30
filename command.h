#ifndef COMMAND_H
#define COMMAND_H

typedef enum cmdtype
{
	usual,
	bg,
	cd,
}
cmdtype_t;

typedef struct command
{
	char **args;
	cmdtype_t type;
	int fd_in;
	int fd_out;
}
command_t;

typedef struct cmdtemp
{
	char **argv;
	int argc;
	int fd_in;
	int fd_out;
	int start;
	int end;
	int cur;
	int count;
	cmdtype_t type;
	int err_flag;
	int *bg_count;
}
cmdtemp_t;

void handle_commands(char **argv, int argc, int *bg_count);

#endif
