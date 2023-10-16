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

void dup_streams(const int fd[])
{
	for(int i = 0; i < 2; i++)
	{
		if(fd[i] != i)
		{
			dup2(fd[i], i);
			close(fd[i]);
		}
	}
}

void close_streams(int fd[])
{
	for(int i = 0; i < 2; i++)
	{
		if(fd[i] != i)
		{
			close(fd[i]);
			fd[i] = i;
		}
	}
}

void crwr_pipe(int *tmp_rdfd, int *wrfd)
{
	int fd_pipe[2];
	pipe(fd_pipe);
	*tmp_rdfd = fd_pipe[0];
	*wrfd = fd_pipe[1];
}

void rd_pipe(int rdpipe, int *rdfd)
{
	*rdfd = rdpipe;
}
