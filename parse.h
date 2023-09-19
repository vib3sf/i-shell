#ifndef PARSE_H
#define PARSE_H

int handle_command(int first_c, char ***argv_buf);

void print_result(int res, int argc, char **argv);

void free_argv(char **argv);

void print_parse_error(int res);

#endif
