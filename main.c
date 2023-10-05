#include "command.h"
#include "parse.h"
#include "exec.h"
#include <stdio.h>

int main()
{
	change_dir(NULL);	/* cd to home dir */
	int argc;
    for(;;) 
	{
        printf("> ");

		char** argv_buf = NULL;
        int res = parse_command(&argv_buf, &argc);

		if (!res) 
			handle_commands(argv_buf, argc);

        free_argv(argv_buf, argc);
    }
    return 0;
}
