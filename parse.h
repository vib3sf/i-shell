#ifndef PARSE_H
#define PARSE_H

/* Rememebers previous char type.
 * Normal chars are letters and digits
 * Specials are &, |, ;, ,  
 * Quotes state is for writing strings.
 * Space is space :)
 */

typedef enum parse_err
{
	quotes_err,
	special_char_err,
	no_err,
}
parse_err_t;

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
	int buf_count;
	char **argv;
	int argc;
	parse_err_t err;
} parse_t;


int parse_command(char ***argv_buf, int *argc);
void free_argv(char **argv, int argc);

#endif
