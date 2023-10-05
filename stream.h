#ifndef STREAM_H
#define STREAM_H

typedef enum stream 
{
	in,
	out,
	append,
}
stream_t;

int change_fd(char *name, stream_t stream, int old_fd);
int dup_streams(int fd_in, int fd_out);
int crwr_pipe(int *fd_out);
void rd_pipe(int pipe_in, int *fd_in);

#endif
