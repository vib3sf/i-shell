#include "command.h"
#include "parse.h"
#include "exec.h"
#include "sig.h"

#include <stdio.h>
#include <signal.h>

static void 
handle_parse_result(parse_res_t res, argument_t *argv_buf, int argc);

int main(void)
{
	argument_t *argv_buf;
	int argc;
	parse_res_t res;

	signal(SIGTTOU, SIG_IGN);
	switch_sigchld_status(handle);

	change_dir(NULL);	/* cd to home dir */

    for(;;) 
	{
        printf("> ");

		argv_buf = NULL;

		signal(SIGINT, SIG_IGN);	/* ingore while reading the command */
        res = parse_command(&argv_buf, &argc);
		signal(SIGINT, SIG_DFL);
		
		handle_parse_result(res, argv_buf, argc);

        free_argv(argv_buf, argc);
    }
    return 0;
}


static void handle_parse_result(parse_res_t res, argument_t *argv_buf, int argc)
{
	switch(res)
	{
		case no_err:
			handle_commands(argv_buf, argc);
			break;
		case empty_cmd:
			break;
		case quotes_err:
			printf("err: unmatched quotes.\n");
			break;
		case special_char_err:
			printf("err: invalid combination of special chars.\n");
			break;
	}

}
