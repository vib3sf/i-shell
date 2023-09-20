#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void add_argv(int *argc, char ***argv, const char *buf, int buf_size);

static void empty_buf(char **buf, int *pbuf_size);

static void extend_buf(char **buf, int *buf_size, char c);


int handle_command(int first_c, char ***argv_buf)
{
    char *buf = malloc(sizeof(char));      /* buffer for args */
	*buf = '\0';

    int c = first_c, buf_size = 1, argc = 0, quotes_count = 0;

    while(c != '\n')
	{
        switch(c) 
		{
            case '\\':
                c = getchar();
                extend_buf(&buf, &buf_size, c);
                break;

            case ' ':
                if (quotes_count % 2 == 1)
				{
                    extend_buf(&buf, &buf_size, c);
                }
                else if(*buf) 
				{
                    add_argv(&argc, argv_buf, buf, buf_size);
                    empty_buf(&buf, &buf_size);
                }
                break;

            case '"':
                quotes_count++;
                break;
                
            default:
                extend_buf(&buf, &buf_size, c);
                break;
        }

        c = getchar();
    } 
    
    if(*buf)
	{
        add_argv(&argc, argv_buf, buf, buf_size);
	}

	free(buf);

    *argv_buf = realloc(*argv_buf, (argc + 1) * sizeof(char *));
	(*argv_buf)[argc] = NULL;

	return (quotes_count % 2 == 1) ? -1 : 0;
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
