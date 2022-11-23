#define _DEFAULT_SOURCE
#define  _BSD_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pwd.h"

char  buffer_pwd[PATH_MAX];
void logical_pwd() {
    printf("%s\n",getenv("PWD"));
    fflush(stdout);
}

void absolute_pwd() {
    getcwd(buffer_pwd,PATH_MAX);
    printf("%s\n",buffer_pwd);
    fflush(stdout);
}
int pwd(int argc, char ** argv) {
    if (argc > 3) return -1;
    if (argc == 1) logical_pwd();
    else if ((strcmp(argv[1],"-L"))==0) logical_pwd();
    else if ((strcmp(argv[1],"-P") == 0)) absolute_pwd();
    else printf("Mauvais format ,format attendu : pwd [-L | -P] \n");
    fflush(stdout);
    return 0;    
}




