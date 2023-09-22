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
static void finish_parse(parse_t *prs, char ***argv_buf);

int parse_command(char ***argv_buf)
{
	parse_t *prs;
	parse_init(&prs);
	
    while((prs->c = getchar()) != '\n')
	{
		handle_char(prs);
	}

    finish_parse(prs, argv_buf);
	return prs->state == quotes;
}

static void parse_init(parse_t **prs)
{
	*prs = malloc(sizeof(parse_t));
    (*prs)->buf = malloc(sizeof(char));      /* buffer for args */
	*(*prs)->buf = '\0';
	(*prs)->state = normal;
	(*prs)->buf_size = 1;
	(*prs)->argc = 0;
	(*prs)->argv = NULL;
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
	{

		printf("Special chars error\n");
		exit(1);
	}

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

static void finish_parse(parse_t *prs, char ***argv_buf)
{
	if(prs->state != space)
	{
        add_argv(prs);
	}

	free(prs->buf);

    *argv_buf = realloc(prs->argv, (prs->argc + 1) * sizeof(char *));
	(*argv_buf)[prs->argc] = NULL;
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
    prs->buf = malloc(sizeof(char));
    *(prs->buf) = '\0';
    prs->buf_size = 1;
}

static void extend_buf(parse_t *prs)
{
    prs->buf = realloc(prs->buf, (prs->buf_size + 1) * sizeof(char));
    (prs->buf)[prs->buf_size - 1] = prs->c;
    (prs->buf)[prs->buf_size] = '\0';
	prs->buf_size++;
}

void free_argv(char **argv)
{
    for(char **tmp = argv; *tmp; tmp++)
        free(*tmp);
    free(argv);
}

void print_parse_error(int res)
{
    printf("Error: unmatched quotes.\n");
}

