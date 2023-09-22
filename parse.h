#ifndef PARSE_H
#define PARSE_H

/* Rememebers previous char type.
 * Normal chars are letters and digits
 * Specials are &, |, ;, ,  
 * Quotes state is for writing strings.
 * Space is space :)
 */

typedef enum char_state 
{
	normal,
	special,
	quotes,
	space
} char_state_t;

typedef struct parse
{
	int c;
	char_state_t state;
	char *buf;
	int buf_size;
	char **argv;
	int argc;
} parse_t;

void parse_init(parse_t **pd);

int parse_command(char ***argv_buf);

void print_result(int res, int argc, char **argv);

void free_argv(char **argv);

void print_parse_error(int res);

#endif
