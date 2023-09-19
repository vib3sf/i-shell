#include "parse.h"
#include "exec.h"
#include <stdio.h>

int main()
{
    for(;;) 
	{
        printf("> ");
        int c = getchar();
        if (c == EOF)
            break;

		char** argv_buf = NULL;
        int res = handle_command(c, &argv_buf);;

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
