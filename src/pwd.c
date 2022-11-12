#define _DEFAULT_SOURCE
#define  _BSD_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char  buffer[PATH_MAX];
void logical_pwd() {
    printf("%s",getenv("PWD"));
}

void absolute_pwd() {
    getcwd(buffer,PATH_MAX);
    printf("%s",buffer);
}
int main(int argc, char ** argv) {
    printf("appl arg = %s \n",argv[0]);
    if (argc > 3) return -1;
    logical_pwd();
    if ((strcmp(argv[2],"-L"))==0) logical_pwd();
    else if ((strcmp(argv[2],"-P") == 0)) absolute_pwd();
    return 0;    
}

