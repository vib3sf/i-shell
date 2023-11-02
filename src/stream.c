#include "stream.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static int get_mode(stream_t stream);

static void fd_iter(int fd[], void (*fptr)(int *, int));
static void dup_stream(int *fd, int i);
static void close_stream(int *fd, int i);

int change_fd(char *name, stream_t stream, int old_fd)
{
	int fd, mode = get_mode(stream);
	if(old_fd != 0 && old_fd != 1)
		close(old_fd);

	fd = open(name, mode, 0666);

	if(fd == -1)
		perror(name);

	return fd;
}

static int get_mode(stream_t stream)
{
	int mode;
	switch(stream)
	{
		case in:
			mode = O_RDONLY|O_CREAT;
			break;
		case out:
			mode = O_WRONLY|O_CREAT|O_TRUNC;
			break;
		case append:
			mode = O_WRONLY|O_CREAT|O_APPEND;
	}
	return mode;
}

void dup_nostd_streams(int fd[])
{
	fd_iter(fd, dup_stream);
}

void close_nostd_streams(int fd[])
{
	fd_iter(fd, close_stream);
}

static void fd_iter(int fd[], void (*fptr)(int *, int))
{
	int i;
	for(i = 0; i < 2; i++)
	{
		if(fd[i] != i)
			fptr(fd + i, i);
	}
}

static void dup_stream(int *fd, int i)
{
	dup2(*fd, i);
	close(*fd);
}


static void close_stream(int *fd, int i)
{
	close(*fd);
	*fd = i;
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
