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
        int argc, res = handle_command(c, &argc, &argv_buf);;

		if (res) 
		{
			print_parse_error(res);
		} 
		else
			exec(argc, argv_buf);

        free_argv(argc, argv_buf);
    }
    return 0;
}
