#include "exec.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <pwd.h>

static int exec_args(char **argv, int start, int end);

static int cr_fork(char **argv, int start, int end);

static char *expand_home_dir(char *path);

static char *get_home_dir();

int exec(char **argv, int *zombie_count)
{
	int fg = 0;
	if(!strcmp(argv[0], "cd")) 
	{
		if (argv[2])
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
	
	int start = 0, cur = 0;
	for(char **p = argv; *p; p++)
	{
		if(!strcmp(*p, "&&"))
		{
			cr_fork(argv, start, cur - 1);
			start = cur + 1;
		}
		else if(!strcmp(*p, "&"))
		{
			cr_fork(argv, start, cur - 1);
			start = cur + 1;
			(*zombie_count)++;
		}
		cur++;
	}

	if(start != cur){
		fg = cr_fork(argv, start, cur);
		
	}

	int status, wr;
	do
	{
		wr = waitpid(fg, &status, WNOHANG);
				
	} while(!wr);

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

static int cr_fork(char **argv, int start, int end)
{
	int p = fork();

	if(p == -1) 
	{
		perror("fork");
		return -1;
	}

	if(p == 0) 
	{
		exec_args(argv, start, end);
		perror(argv[0]);
		return -1;
	}
	
	return p;

}

static int exec_args(char **argv, int start, int end)
{
	int size = end - start + 1;
	char **cmdline = malloc(size * sizeof(char*));
	for(int i = 0; i < size; i++) {
		cmdline[i] = argv[start + i];
	}
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
