#define  _BSD_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <dirent.h>


char buffer [PATH_MAX];
int cd (char * path) {
    int d = -1;
    if (strcmp(path,"") == 0) {
        d = chdir(getenv("HOME"));  
    }
    else {
        d = chdir(path);
    }
    if (d == -1) {
        perror("chdir");
        return 1;
    }
    getcwd(buffer,PATH_MAX);
    setenv("PWD",buffer,1);
    printf(" PWD = %s\n",getenv("PWD"));
    

    return 0;

}

int main(int argc, char ** argv) {
    if (argc > 3) return -1;
    if (argc == 1) return cd("");
    // Cas à faire pour "cd vide"
    printf("PWD = %s\n",getenv("PWD"));
    return cd(argv[1]);
}
