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
void dup_streams(int rdfd, int wrfd);
void close_streams(int *rdfd, int *wrfd);

void crwr_pipe(int *rdfd, int *wrfd);
void rd_pipe(int rdpipe, int *rdfd);

#endif
