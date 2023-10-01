#include "stream.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int change_fd(char *name, stream_t stream, int old_fd)
{
	int mode;
	switch(stream)
	{
		case in:
			mode = O_RDONLY|O_CREAT;
			if(old_fd != 0)
				close(old_fd);
			break;
		case out:
			if(old_fd != 1)
				close(old_fd);
			mode = O_WRONLY|O_CREAT|O_TRUNC;
			break;
		case append:
			if(old_fd != 1)
				close(old_fd);
			mode = O_WRONLY|O_CREAT|O_APPEND;

	}
	int fd = open(name, mode, 0666);

	if(fd == -1)
		perror(name);

	return fd;
}

int dup_streams(int fd_in, int fd_out)
{
	if(fd_in != 0){
		dup2(fd_in, 0);
		close(fd_in);
	}

	if(fd_out != 1){
		dup2(fd_out, 1);
		close(fd_out);
	}

	return 0;
}
