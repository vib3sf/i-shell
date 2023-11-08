#include "parse.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* inits pd with starting value */
static void parse_init(parse_t **prs);

/* array memory funcs */
static void add_argv(parse_t *prs);
static void empty_buf(parse_t *prs);
static void extend_buf(parse_t *prs);

static arg_type_t get_arg_type(parse_t *prs);
static arg_type_t get_special_arg_type(parse_t *prs);

/* handlers controller */
static void handle_char(parse_t *prs);

/* handles chars funcs */
static void handle_slash(parse_t *prs);
static void handle_space(parse_t *prs);
static void handle_quote(parse_t *prs);
static void handle_special_char(parse_t *prs);
static int special_is_not_valid(parse_t *prs);
static void handle_normal_char(parse_t *prs);

/* adds last arg value and cleans memory */
static void finish_parse(parse_t *prs, argument_t **argv_buf, int *argc);

#define START_BUF_SIZE 4;

parse_res_t parse_command(argument_t **argv_buf, int *argc)
{
	parse_t *prs;
	parse_init(&prs);

    while((prs->c = getchar()) != '\n')
	{
		if(prs->c == -1 && errno == EINTR)
		{
			errno = 0;
			continue;
		}

		if(prs->res != no_err)
			break;
		handle_char(prs);
	}

    finish_parse(prs, argv_buf, argc);
	return prs->res;
}

static void parse_init(parse_t **prs)
{
	*prs = malloc(sizeof(parse_t));
	(*prs)->buf_size = START_BUF_SIZE;
	(*prs)->buf_count = 1;
    (*prs)->buf = malloc(sizeof(char) * (*prs)->buf_size);      /* buffer for args */
	*(*prs)->buf = '\0';
	(*prs)->state = normal;
	(*prs)->argc = 0;
	(*prs)->argv = NULL;
	(*prs)->res = no_err;
}

static void handle_char(parse_t *prs)
{
	switch(prs->c) 
	{
		case EOF:
			exit(0);

		case '\\':
			handle_slash(prs); 
			break;

	   case ' ':
			handle_space(prs); 
			break;

		case '"':
			handle_quote(prs); 
			break;

		case '&': case '|': case '>': case '<': 
			handle_special_char(prs);
			break;

		default:
			handle_normal_char(prs);
	} 
}

static void handle_slash(parse_t *prs)
{
	prs->c = getchar();
	extend_buf(prs);
}

static void handle_space(parse_t *prs)
{
	if (prs->state == quotes)
	{
		extend_buf(prs);
		return;
	}
	else if(*prs->buf) 
	{
		add_argv(prs);
		empty_buf(prs);
	}
	prs->state = space;
}

static void handle_quote(parse_t *prs)
{
	prs->state = (prs->state != quotes) ? quotes : normal;
}

static void handle_special_char(parse_t *prs)
{
	if (prs->state == quotes)
	{
		extend_buf(prs);
		return;
	}

	if(prs->state == normal){
		add_argv(prs);
		empty_buf(prs);
	}

	/* special args may have max 2 chars and chars as &, | and > */
	if(special_is_not_valid(prs))
		prs->res = special_char_err;

	prs->state = special;
	extend_buf(prs);

}

static int special_is_not_valid(parse_t *prs)
{
	return prs->state == special && 
		(prs->c == prs->buf[0] || strlen(prs->buf) > 1) &&
		!((prs->c == '&' || prs->c == '|' || prs->c == '>'));
}

static void handle_normal_char(parse_t *prs)
{
	if(prs->state == special)
	{
		add_argv(prs);
		empty_buf(prs);
	}
	
	if(prs->state != quotes)
		prs-> state = normal;
	
	extend_buf(prs);
}

static void finish_parse(parse_t *prs, argument_t **argv_buf, int *argc)
{
	if(prs->state != space && *prs->buf)
	{
        add_argv(prs);
	}

	free(prs->buf);

	*argc = prs->argc;
	*argv_buf = prs->argv;

	if (prs->state == quotes)
		prs->res = quotes_err;
	else if(prs->argc == 0)
		prs->res = empty_cmd;

	free(prs);
}

static void add_argv(parse_t *prs)
{	
	char **arg;
    prs->argv = realloc(prs->argv, (prs->argc + 1) * sizeof(argument_t));

    arg = &(prs->argv)[prs->argc].s;	
    *arg = malloc(prs->buf_size * sizeof(char));
    strcpy(*arg, prs->buf);

	prs->argv[prs->argc].type = get_arg_type(prs);

    prs->argc++;
}

static arg_type_t get_arg_type(parse_t *prs)
{
	arg_type_t res;
	switch(prs->state)
	{
		case special:
			res = get_special_arg_type(prs);
			break;
		case normal: case quotes: case space:
			res = word_arg;
	}
	return res;
}

static arg_type_t get_special_arg_type(parse_t *prs)
{
	arg_type_t res;
	switch(prs->buf[0])
	{
		case '&':
			res = (strlen(prs->buf) == 2) ? logical_and_arg : bg_arg;
			break;
		case '|':
			res = (strlen(prs->buf) == 2) ? logical_or_arg : pipe_arg;
			break;
		case '>':
			res = (strlen(prs->buf) == 2) ? append_arg : out_arg;
			break;
		case '<':
			res = in_arg;
			break;
	}
	return res;
}

static void empty_buf(parse_t *prs)
{
    free(prs->buf);
    prs->buf_size = START_BUF_SIZE;
	prs->buf_count = 1;
    prs->buf = malloc(sizeof(char) * prs->buf_size);
    *(prs->buf) = '\0';
}

static void extend_buf(parse_t *prs)
{
	if(prs->buf_count == prs->buf_size)
	{
		prs->buf_size *= 2;
		prs->buf = realloc(prs->buf, prs->buf_size * sizeof(char));
	}
    (prs->buf)[prs->buf_count - 1] = prs->c;
    (prs->buf)[prs->buf_count] = '\0';
	prs->buf_count++;
}

void free_argv(argument_t *argv, int argc)
{
	int i;
    for(i = 0; i < argc; i++)
        free(argv[i].s);
    free(argv);
}

