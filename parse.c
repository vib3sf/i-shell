#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* inits pd with starting value */
static void parse_init(parse_t **prs);

/* array memory funcs */
static void add_argv(parse_t *prs);
static void empty_buf(parse_t *prs);
static void extend_buf(parse_t *prs);

/* handlers controller */
static void handle_char(parse_t *prs);

/* handles chars funcs */
static void handle_slash(parse_t *prs);
static void handle_space(parse_t *prs);
static void handle_quote(parse_t *prs);
static void handle_special_char(parse_t *prs);
static void handle_normal_char(parse_t *prs);

/* adds last arg value and cleans memory */
static void finish_parse(parse_t *prs, char ***argv_buf, int *argc);
static int check_errors(parse_err_t err);

#define START_BUF_SIZE 4;

int parse_command(char ***argv_buf, int *argc)
{
	parse_t *prs;
	parse_init(&prs);
	
    while((prs->c = getchar()) != '\n')
	{
		if(prs->err != no_err)
			break;
		handle_char(prs);
	}

    finish_parse(prs, argv_buf, argc);
	return check_errors(prs->err);
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
	(*prs)->err = no_err;
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

		case '&': case '|': case '>': case '<': case ';': case '(': case ')':
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
	else if(*(prs->buf)) 
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
	if((prs->state == special && 
			!((prs->c == '&' || prs->c == '|' || prs->c == '>') && prs->c == prs->buf[0])) ||
			strlen(prs->buf) > 1) 
		prs->err = special_char_err;

	prs->state = special;
	extend_buf(prs);

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

static void finish_parse(parse_t *prs, char ***argv_buf, int *argc)
{
	if(prs->state != space)
	{
        add_argv(prs);
	}

	free(prs->buf);

	*argc = prs->argc;
	*argv_buf = prs->argv;

	if (prs->state == quotes)
		prs->err = quotes_err;
}

static int check_errors(parse_err_t err)
{
	switch(err)
	{
		case no_err:
			return 0;
		case quotes_err:
			printf("err: unmatched quotes.\n");
			return 1;
		case special_char_err:
			printf("err: invalid combination of special chars.\n");
			return 2;
	}
}

static void add_argv(parse_t *prs)
{
    prs->argv = realloc(prs->argv, (prs->argc + 1) * sizeof(char *));

    char **p = &(prs->argv)[prs->argc];	/* The great mystery of humanity */
    *p = malloc(prs->buf_size * sizeof(char));
    strcpy(*p, prs->buf);

    prs->argc++;
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

void free_argv(char **argv, int argc)
{
    for(int i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);
}

