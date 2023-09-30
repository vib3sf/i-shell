#include "exec.h"
#include "stream.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <pwd.h>

static void exec_bg(command_t *cmd);
static void exec_usual(command_t *cmd);

static int exec_args(command_t *cmd);
static int cr_fork(command_t *cmd);
static char *expand_home_dir(char *path);
static char *get_home_dir();

int exec(command_t *cmd)
{
	switch(cmd->type)
	{
		case cd:
			change_dir(cmd->args[1]);
			break;
		case usual:
			exec_usual(cmd);
			break;
		case bg:
			exec_bg(cmd);
	}
	return 0;
}

int change_dir(char *path)
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

	return chd;
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


static void exec_usual(command_t *cmd)
{
	int wr;
	int pid = cr_fork(cmd);

	do 
		wr = waitpid(pid, NULL, WNOHANG);
	while(!wr);
}

static void exec_bg(command_t *cmd)
{
	cr_fork(cmd);
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
		exec_args(cmd);
		perror(*cmd->args);
		return -1;
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

static int exec_args(command_t *cmd)
{
	execvp(*cmd->args, cmd->args);
	perror(*cmd->args);
	_exit(1);
}

void wait_bgs(int *bg_count)
{
	int i = *bg_count, wr;
	while (i){
		wr = waitpid(-1, NULL, WNOHANG);
		if (wr)
		{
			(*bg_count)--;
		}
		i--;
	}
}
