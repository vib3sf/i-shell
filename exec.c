#include "exec.h"
#include "command.h"
#include "stream.h"
#include "sig_handlers.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
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
		case usual:
			return exec_usual(cmd);
		case bg: case pip:
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
	
	if(pid == -1)
		return fork_err;

	int wr, status;
	do 
		wr = wait(&status);
	while(wr != pid);

	switch_sigchld_status(handle);

	return (WEXITSTATUS(status) || WIFSIGNALED(status)) ? 
		exec_err : no_cmd_err;
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
		exec_argv(cmd);

	close_streams(&cmd->fd_in, &cmd->fd_out);

	return pid;
}

static int exec_argv(command_t *cmd)
{
	if(cmd->pipe_in_tmp != 0)
		close(cmd->pipe_in_tmp);

	if(cmd->type == usual)
	{
		int pid = getpid();
		if(cmd->pgid != 0)
		{
			setpgid(pid, cmd->pgid);
		}
		else
		{
			setpgid(pid, pid);
			cmd->pgid = pid;
			tcsetpgrp(0, pid);
		}
	}

	dup_streams(cmd->fd_in, cmd->fd_out);
	execvp(*cmd->argv, cmd->argv);
	perror(*cmd->argv);
	_exit(1);
}


