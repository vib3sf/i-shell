#include "exec.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <pwd.h>

static int cr_proc(command_t *cmd);

static int exec_cd(char **argv);
static int exec_commands(char **argv, int *zombie_count);
static void exec_bg_cmd(command_t *cmd, int *zombie_count);
static void exec_cmd(command_t *cmd);

static int exec_args(command_t *cmd);
static int cr_fork(command_t *cmd);
static char *expand_home_dir(char *path);
static char *get_home_dir();

int exec(char **argv, int *zombie_count)
{
	int result;
	if(!strcmp(argv[0], "cd")) 
	{
		result = exec_cd(argv);	
	}
	else {
		result = exec_commands(argv, zombie_count);
	}

	return result;
}

static int exec_cd(char **argv)
{
	if (argv[2] && argv[1])
	{
		printf("Error: cd have only 1 arg\n");
		return -1;
	}
	int chd = change_dir(argv[1]);

	if(chd) {
		perror(argv[1]);
		return -1;
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


static int exec_commands(char **argv, int *zombie_count)
{
	command_t *cmd = malloc(sizeof(command_t));

	cmd->argv = argv;
	cmd->start = 0;
	cmd->cur = 0;

	for(char **p = argv; *p; p++)
	{
		if(!strcmp(*p, "&&"))
		{
			exec_cmd(cmd);
		}
		else if(!strcmp(*p, "&"))
		{
			exec_bg_cmd(cmd, zombie_count);
		}
		cmd->cur++;
	}

	if(cmd->start != cmd->cur){
		cmd->cur++;
		exec_cmd(cmd);
	}

	free(cmd);

	return 0;
}

static void exec_cmd(command_t *cmd)
{
	int wr;
	cmd->cur_pid = cr_proc(cmd);

	do 
		wr = waitpid(cmd->cur_pid, NULL, WNOHANG);
	while(!wr);
}

static void exec_bg_cmd(command_t *cmd, int *zombie_count)
{
	cr_proc(cmd);
	(*zombie_count)++;
}

static int cr_proc(command_t *cmd)
{
	int pid = cr_fork(cmd);
	cmd->start = cmd->cur + 1;
	return pid;
}

static int cr_fork(command_t *cmd)
{
	int p = fork();

	if(p == -1) 
	{
		perror("fork");
		return -1;
	}

	if(p == 0) 
	{
		exec_args(cmd);
		perror(cmd->argv[0]);
		return -1;
	}
	
	return p;

}

static int exec_args(command_t *cmd)
{
	int size = cmd->cur - cmd->start;
	char **cmdline = malloc((size + 1) * sizeof(char*));
	for(int i = 0; i < size; i++) {
		cmdline[i] = cmd->argv[cmd->start + i];
	}
	cmdline[size] = NULL;
	execvp(cmdline[0], cmdline);
	perror(cmdline[0]);
	_exit(1);
}

void wait_zombies(int *zombie_count)
{
	int i = *zombie_count, wr;
	while (i){
		wr = waitpid(-1, NULL, WNOHANG);
		if (wr)
		{
			(*zombie_count)--;
		}
		i--;
	}
}
