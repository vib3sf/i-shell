#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void add_argv(int *argc, char ***argv, const char *buf, int buf_size);

static void empty_buf(char **buf, int *pbuf_size);

static void extend_buf(char **buf, int *buf_size, char c);

int parse_command(char ***argv_buf)
{
    char *buf = malloc(sizeof(char));      /* buffer for args */
	*buf = '\0';

    int c, buf_size = 1, argc = 0;
	char_state_t state;
	

    while((c = getchar()) != '\n')
	{
        switch(c) 
		{
			case EOF:
				exit(0);
            case '\\':
                c = getchar();
                extend_buf(&buf, &buf_size, c);
                break;
           case ' ':
                if (state == quotes)
				{
                    extend_buf(&buf, &buf_size, c);
                }
                else if(*buf) 
				{
                    add_argv(&argc, argv_buf, buf, buf_size);
                    empty_buf(&buf, &buf_size);
                }
				state = space;
                break;

            case '"':
				state = (state == quotes) ? normal : quotes;
                break;
                
			case '&': case '|': case '>': case '<': case ';': case '(': case ')':
				if (state == quotes)
				{
					extend_buf(&buf, &buf_size, c);
					break;
				}

				if(state == normal){
                    add_argv(&argc, argv_buf, buf, buf_size);
                    empty_buf(&buf, &buf_size);
				}

				/* special args may have max 2 chars and chars as &, | and > */
				if((state == special && 
						!((c == '&' || c == '|' || c == '>') && c == buf[0])) ||
						strlen(buf) > 1) 
				{

					printf("Special chars error\n");
					exit(1);
				}

				state = special;
				extend_buf(&buf, &buf_size, c);
				break;

            default:
				if(state == special)
				{
					add_argv(&argc, argv_buf, buf, buf_size);
					empty_buf(&buf, &buf_size);
					break;
				}
				state = normal;
                extend_buf(&buf, &buf_size, c);
                break;
        }
    } 
    
    if(state != space)
	{
        add_argv(&argc, argv_buf, buf, buf_size);
	}

	free(buf);

    *argv_buf = realloc(*argv_buf, (argc + 1) * sizeof(char *));
	(*argv_buf)[argc] = NULL;

	return state == quotes;
}

static void add_argv(int *pargc, char ***pargv, const char *buf, int buf_size)
{
    *pargv = realloc(*pargv, (*pargc + 1) * sizeof(char *));

    char **p = &(*pargv)[*pargc];	/* The great mystery of humanity */
    *p = malloc(buf_size * sizeof(char*));
    strcpy(*p, buf);

    (*pargc)++;
}

static void empty_buf(char **pbuf, int *pbuf_size)
{
    free(*pbuf);
    *pbuf = malloc(sizeof(char));
    **pbuf = '\0';
    *pbuf_size = 1;
}

void free_argv(char **argv)
{
	int i = 0;
    for(char **tmp = argv; *tmp; tmp++, i++)
        free(*tmp);
    free(argv);
}

static void extend_buf(char **buf, int *pbuf_size, char c)
{
    *buf = realloc(*buf, (*pbuf_size + 1) * sizeof(char));
    (*buf)[*pbuf_size - 1] = c;
    (*buf)[*pbuf_size] = '\0';
    (*pbuf_size)++;
}

void print_parse_error(int res)
{
    printf("Error: unmatched quotes.\n");
}
