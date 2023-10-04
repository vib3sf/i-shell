#include "exec.h"
#include "command.h"
#include "stream.h"
#include "sig_handlers.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <pwd.h>

static cmd_err_t exec_bg(command_t *cmd);
static cmd_err_t exec_usual(command_t *cmd);

static int exec_argv(command_t *cmd);
static int cr_fork(command_t *cmd);
static char *expand_home_dir(char *path);
static char *get_home_dir();

cmd_err_t exec(command_t *cmd)
{
	switch(cmd->type)
	{
		case cd:
			return change_dir(cmd->argv[1]);
			break;
		case usual:
			return exec_usual(cmd);
			break;
		case bg:
			return exec_bg(cmd);
	}
}

cmd_err_t change_dir(char *path)
{
	int chd;

	if(path == NULL)
	{
		chd = chdir(get_home_dir());
	}
	else if(path[0] == '~')
	{
		char *new_path = expand_home_dir(path);
		chd = chdir(new_path);
		free(new_path);
	}
	else
		chd = chdir(path);

	if(chd == -1)
	{
		perror(path);
		return cd_err;
	}

	return no_cmd_err;
}

static char *expand_home_dir(char *path)
{
	char *hd = get_home_dir();
	int path_size = strlen(hd) + strlen(path) + 2;

	char *path_buf = malloc(path_size * sizeof(char));
	strcpy(path_buf, hd);
	strcat(path_buf, path + 1);

	return path_buf;
}

static char *get_home_dir()
{
	return getpwuid(getuid())->pw_dir;
}


static cmd_err_t exec_usual(command_t *cmd)
{
	switch_sigchld_status(ignore);
	int pid = cr_fork(cmd);
	
	if(pid == -1){
		perror("fork");
		return fork_err;
	}

	int wr, status;
	do 
		wr = waitpid(pid, &status, WNOHANG);
	while(!wr);

	switch_sigchld_status(handle);

	return (WEXITSTATUS(status)) ? exec_err : no_cmd_err;
}

static cmd_err_t exec_bg(command_t *cmd)
{
	int pid = cr_fork(cmd);
	
	return (pid == -1) ? fork_err : no_cmd_err;
}

static int cr_fork(command_t *cmd)
{
	int pid = fork();
	
	if(pid == -1) 
	{
		perror("fork");
		return -1;
	}

	if(pid == 0) 
	{
		dup_streams(cmd->fd_in, cmd->fd_out);
		exec_argv(cmd);
		perror(*cmd->argv);
		exit(1);
	}

	if(cmd->fd_in != 0){
		close(cmd->fd_in);
		cmd->fd_in = 0;
	}

	if(cmd->fd_out != 1){
		close(cmd->fd_out);
		cmd->fd_in = 1;
	}
	
	return pid;

}

static int exec_argv(command_t *cmd)
{
	execvp(*cmd->argv, cmd->argv);
	perror(*cmd->argv);
	_exit(1);
}


