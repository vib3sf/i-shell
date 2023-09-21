#include "exec.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <pwd.h>

static char *expand_home_dir(char *path);

static char *get_home_dir();

int exec(char **argv)
{
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

	int p = fork();

	if(p == -1) 
	{
		perror("fork");
		return -1;
	}

	if(p == 0) 
	{
		execvp(argv[0], argv);
		perror(argv[0]);
		return -1;
	}
	
	int status, wr;
	wr = wait(&status);

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
