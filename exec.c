#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

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
		execvp(argv[0], argv);
		perror(argv[0]);
		return -1;
	}
	
	int status, wr;
	wr = wait(&status);

	return 0;
	
}
