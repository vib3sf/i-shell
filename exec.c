#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

int exec(char **argv)
{
	int p = fork();

	if(p == -1) 
	{
		perror("fork");
		return -1;
	}

	if(p == 0) 
	{
		if(!strcmp(argv[0], "cd")) 
		{
			if (argv[2])
			{
				printf("Error: cd have only 1 arg");
				return -1;
			}

			int chd = chdir(argv[1]);
			if (chd == -1) 
			{
				perror(argv[1]);
				return -1;
			}
			return 0;
		}
		execvp(argv[0], argv);
		perror(argv[0]);
		return -1;
	}
	
	int status, wr;
	wr = wait(&status);

	return 0;
	
}
