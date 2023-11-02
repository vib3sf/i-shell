#ifndef STREAM_H
#define STREAM_H

typedef enum stream 
{
	in,
	out,
	append
}
stream_t;

/* for working with stream redirections */
int change_fd(char *name, stream_t stream, int old_fd);
void dup_nostd_streams(int fd[]);
void close_nostd_streams(int fd[]);

/* for working with pipes */
void crwr_pipe(int *tmp_rdfd, int *wrfd);
void rd_pipe(int rdpipe, int *rdfd);

#endif
