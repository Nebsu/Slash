#define _DEFAULT_SOURCE
#define  _BSD_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pwd.h"

char  buffer_pwd[PATH_MAX];
void logical_pwd(int * redirfd) {
    dprintf(*redirfd,"%s\n",getenv("PWD"));
}

void absolute_pwd(int * redirfd) {
    getcwd(buffer_pwd,PATH_MAX);
    dprintf(*redirfd,"%s\n",buffer_pwd);
}

int pwd(int argc, char ** argv, int * redirfd) {
    if (argc > 3) return -1;
    if (argc == 1) logical_pwd(redirfd);
    else if ((strcmp(argv[1],"-L"))==0) logical_pwd(redirfd);
    else if ((strcmp(argv[1],"-P") == 0)) absolute_pwd(redirfd);
    else printf("Mauvais format ,format attendu : pwd [-L | -P] \n");
    return 0;    
}