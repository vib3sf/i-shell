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

void dup_streams(int rdfd, int wrfd)
{
	if(rdfd != 0)
	{
		dup2(rdfd, 0);
		close(rdfd);
	}

	if(wrfd != 1)
	{
		dup2(wrfd, 1);
		close(wrfd);
	}
}

void close_streams(int *rdfd, int *wrfd)
{
	if(*rdfd != 0)
	{
		close(*rdfd);
		rdfd = 0;
	}

	if(*wrfd != 1)
	{
		close(*wrfd);
		*wrfd = 1;
	}
}

void crwr_pipe(int *rdfd, int *wrfd)
{
	int fd[2];
	pipe(fd);
	*rdfd = fd[0];
	*wrfd = fd[1];
}

void rd_pipe(int rdpipe, int *rdfd)
{
	*rdfd = rdpipe;
}
