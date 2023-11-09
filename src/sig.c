#include "sig.h"

#include <signal.h>
#include <wait.h>
#include <unistd.h>

void switch_sigchld_status(sigchld_status_t st)
{
	switch(st)
	{
		case ignore:
			signal(SIGCHLD, SIG_DFL);
			return;
		case handle:
			signal(SIGCHLD, handle_kill_bg);
	}
}

void handle_kill_bg(int s)
{
	int pid;

	signal(SIGCHLD, handle_kill_bg);

	do 
	{
		pid = waitpid(-1, NULL, WNOHANG);
	} 
	while(pid > 0);

	tcsetpgrp(0, getpid());		/* returns controlling terminal after exec */
}

