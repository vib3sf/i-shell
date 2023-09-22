#include "parse.h"
#include "exec.h"
#include <stdio.h>

int main()
{
	change_dir(NULL);	/* cd to home dir */
    for(;;) 
	{
        printf("> ");

		char** argv_buf = NULL;
        int res = parse_command(&argv_buf);;

		if (res) 
		{
			print_parse_error(res);
		} 
		else
			exec(argv_buf);

        free_argv(argv_buf);
    }
    return 0;
}
