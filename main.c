#include "command.h"
#include "parse.h"
#include "exec.h"
#include <stdio.h>

int main()
{
	change_dir(NULL);	/* cd to home dir */
	int bg_count = 0, argc;
    for(;;) 
	{
		wait_bgs(&bg_count);
        printf("> ");

		char** argv_buf = NULL;
        int res = parse_command(&argv_buf, &argc);

		if (res) 
			print_parse_error(res);
		else
			handle_commands(argv_buf, argc, &bg_count);

        free_argv(argv_buf, argc);
    }
    return 0;
}
