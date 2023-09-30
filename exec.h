#ifndef EXEC_H
#define EXEC_H

#include "command.h"
int exec(command_t *cmd);

/* Changes directory to absolute or relative path (null path is home user directory)*/
int change_dir(char *path);

void wait_bgs(int *bg_count);

#endif
