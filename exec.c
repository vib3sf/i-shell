#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int exec(int argc, char **argv)
{
	int p = fork();

	if(p == -1) 
	{
		perror("fork");
		exit(1);
	}

	if(p == 0) 
	{
		execvp(argv[0], argv);
	}
	
	int status, wr;
	wr = wait(&status);

	return 0;
	
}
