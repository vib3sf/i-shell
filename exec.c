#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <pwd.h>

static void replace_home_dir(char **path);

int exec(char **argv)
{
	if(!strcmp(argv[0], "cd")) 
	{
		if (argv[2])
		{
			printf("Error: cd have only 1 arg");
			return -1;
		}
		else if(argv[1] == NULL || argv[1][0] == '~')
		{
			replace_home_dir(&argv[1]);
		}

		int chd = chdir(argv[1]);
		if (chd == -1) 
		{
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

static void replace_home_dir(char **path)
{
	struct passwd *pw = getpwuid(getuid());

	int path_size = strlen(pw->pw_dir);
	if(*path)
	{
		path_size += strlen(*path) + 2;
	}

	char *path_buf = malloc(path_size * sizeof(char));
	strcpy(path_buf, pw->pw_dir);

	if(*path)
	{
		strcat(path_buf, *(path) + 1);
		free(*path);
	}
	*path = path_buf;
}
