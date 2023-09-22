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

int parse_command(char ***argv_buf);

void print_result(int res, int argc, char **argv);

void free_argv(char **argv);

void print_parse_error(int res);

#endif
