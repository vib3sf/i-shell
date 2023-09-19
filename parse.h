#ifndef PARSE_H
#define  PARSE_H

int handle_command(int first_c, int *argc_out, char ***argv_buf);

void print_result(int res, int argc, char **argv);

void free_argv(int argc, char **argv);

#endif
