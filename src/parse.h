#ifndef PARSE_H
#define PARSE_H

typedef enum arg_type
{
	word_arg,
	logical_and_arg,
	logical_or_arg,
	bg_arg,
	pipe_arg,
	in_arg,
	out_arg,
	append_arg,
}
arg_type_t;

typedef struct argument
{
	char *s;
	arg_type_t type;
}
argument_t;

typedef enum parse_res
{
	quotes_err,
	special_char_err,
	no_err,
	empty_cmd,
}
parse_res_t;

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
	int buf_count;
	argument_t *argv;
	int argc;
	parse_res_t res;
} parse_t;

int parse_command(argument_t **argv_buf, int *argc);
void free_argv(argument_t *argv, int argc);


#endif
