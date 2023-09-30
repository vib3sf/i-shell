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
void return_std_streams(int save_in, int save_out);

#endif
