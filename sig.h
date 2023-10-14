#ifndef SIG_HANDLERS_H
#define SIG_HANDLERS_H

typedef enum sigchld_status
{
	ignore,
	handle
}
sigchld_status_t;

void switch_sigchld_status(sigchld_status_t st);
void handle_kill_bg(int s);

#endif
