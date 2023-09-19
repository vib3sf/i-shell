#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_argv(int argc, char **argv);

static void print_quotes_error();

static void add_argv(int *argc, char ***argv, char *buf, int buf_size);

static void free_buf(char **buf, int *pbuf_size);

static void extend_buf(char **buf, int *buf_size, char c);


int handle_command(int first_c, int *argc_out, char ***argv_buf)
{
    char *buf = NULL;      /* buffer for args */
    int c = first_c, buf_size = 0, quotes_count = 0;

	*argc_out = 0;
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
                else if(buf) 
				{
                    add_argv(argc_out, argv_buf, buf, buf_size);
                    free_buf(&buf, &buf_size);
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
    
    if (buf)
        add_argv(argc_out, argv_buf, buf, buf_size);

    free_buf(&buf, &buf_size);

	return (quotes_count % 2 == 1) ? -1 : 0;
}

static void add_argv(int *pargc, char ***pargv, char *buf, int buf_size)
{
    *pargv = realloc(*pargv, (*pargc + 1) * sizeof(char *));

    char **p = &(*pargv)[*pargc];
    *p = malloc(buf_size + 1);
    strcpy(*p, buf);

    (*p)[buf_size] = '\0';

    (*pargc)++;
}

static void free_buf(char **pbuf, int *pbuf_size)
{
    free(*pbuf);
    *pbuf = NULL;
    *pbuf_size = 0;
}

void free_argv(int argc, char **argv)
{
    for(int i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);
}

static void extend_buf(char **buf, int *pbuf_size, char c)
{
    *buf = realloc(*buf, (*pbuf_size + 1) * sizeof(char));
    (*buf)[*pbuf_size] = c;
    (*pbuf_size)++;
}


void print_result(int res, int argc, char **argv)
{
    switch(res) {
        case 0:
            print_argv(argc, argv);
            break;
        case -1:
            print_quotes_error();
    }
}

void print_argv(int argc, char **argv)
{
    for(int i = 0; i < argc; i++) {
        printf("[%s]\n", argv[i]);
    }
}

static void print_quotes_error()
{
    printf("Error: unmatched quotes.\n");
}
