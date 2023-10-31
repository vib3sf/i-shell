#ifndef PARSE_H
#define PARSE_H

/* all types of args are listed here */

typedef enum arg_type
{
	/* usual arg */
	word_arg,

	/* && and || args */
	logical_and_arg,
	logical_or_arg,

	/* arg whih indicates exec in bg mode */
	bg_arg,
	pipe_arg,

	/* stream redirectioin args */
	in_arg,
	out_arg,
	append_arg,
}
arg_type_t;

/* command node */
typedef struct argument
{
	char *s;
	arg_type_t type;
}
argument_t;

typedef enum parse_res
{
	no_err,
	quotes_err,
	special_char_err,
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
} 
char_state_t;

typedef struct parse
{
	/* reading char data */
	int c;
	char_state_t state;

	/* buffer for parsing commands */
	char *buf;
	int buf_size;
	int buf_count;

	/* result of parsing */
	argument_t *argv;
	int argc;
	parse_res_t res;
} 
parse_t;

int parse_command(argument_t **argv_buf, int *argc);
void free_argv(argument_t *argv, int argc);

#endif
