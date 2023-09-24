#ifndef EXEC_H
#define EXEC_H

int exec(char **argv, int *zombie_count);

/* Changes directory to absolute or relative path (null path is home user directory)*/
int change_dir(char *path);

void wait_zombies(int *zombie_count);

#endif
