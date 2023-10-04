#include "sig_handlers.h"

#include <signal.h>
#include <wait.h>
#include <unistd.h>


void switch_sigchld_status(sigchld_status_t st)
{
	switch(st)
	{
		case ignore:
			signal(SIGCHLD, SIG_DFL);
		case handle:
			signal(SIGCHLD, handle_kill_bg);
	}
}

void handle_kill_bg(int s)
{
	waitpid(-1, NULL, WNOHANG);
}
