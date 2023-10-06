#include "stream.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static int get_mode(stream_t stream);

int change_fd(char *name, stream_t stream, int old_fd)
{
	int mode = get_mode(stream);
		if(old_fd != 0 && old_fd != 1)
		close(old_fd);

	int fd = open(name, mode, 0666);

	if(fd == -1)
		perror(name);

	return fd;
}

static int get_mode(stream_t stream)
{
	switch(stream)
	{
		case in:
			return O_RDONLY|O_CREAT;
		case out:
			return O_WRONLY|O_CREAT|O_TRUNC;
		case append:
			return O_WRONLY|O_CREAT|O_APPEND;
	}
}

void dup_streams(int fd_in, int fd_out)
{
	if(fd_in != 0)
	{
		dup2(fd_in, 0);
		close(fd_in);
	}

	if(fd_out != 1)
	{
		dup2(fd_out, 1);
		close(fd_out);
	}
}

void close_streams(int *fd_in, int *fd_out)
{
	if(*fd_in != 0)
	{
		close(*fd_in);
		fd_in = 0;
	}

	if(*fd_out != 1)
	{
		close(*fd_out);
		*fd_out = 1;
	}
}

void crwr_pipe(int *fd_in, int *fd_out)
{
	int fd[2];
	pipe(fd);
	*fd_in = fd[0];
	*fd_out = fd[1];
}

void rd_pipe(int pipe_in, int *fd_in)
{
	*fd_in = pipe_in;
}
