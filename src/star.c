#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#include "mystring.h"
#include "pile.h"


char buff [4096] [4096];
char * pp = "..";

int isFormat(char * str, char * regEx) {
    int star = 0;
    int strLen = strlen(str);
    int regLen = strlen(regEx);
    int idxStr = strLen-1;
    int idxReg = regLen-1;
    while ( idxStr >= 0 && idxReg >= 0 ) {
        // printf("char str = %c char regEx = %c \n",str[idxStr],regEx[idxReg]);
        if (regEx[idxReg] == '*') {
            return 1;
        }
        if (str[idxStr] != regEx[idxReg] ) {
            return 0;
        }
        else {
            idxStr--;
            idxReg--;
        }
    }
    return idxStr == -1 && idxReg == -1;
}


int oe (int fd,int i,struct string * path,char ** cmd) {
    DIR * dir = NULL;
    struct dirent * entry;
    dir = fdopendir(fd);
    struct string * s = string_new(PATH_MAX);
    if (strcmp(path -> data,".") !=  0) {
      string_append(s,path -> data);
      string_append(s,"/");
    }
    pile * p = pile_init();
    while (entry = readdir(dir)) {
        if(strcmp(entry -> d_name,"..") == 0 || strcmp(entry -> d_name,".") == 0 || entry -> d_name[0] == '.') {
            continue;
        }
        if (entry -> d_type == DT_DIR) {
            struct string * new = string_new(PATH_MAX);
            string_append(new,path -> data);
            string_append(new,"/");
            string_append(new,entry-> d_name);
            // empile(p,elem_init(new));
        }
            string_append(s,entry-> d_name);
            strcpy(cmd[i++],s -> data);
            string_truncate(s,strlen(entry -> d_name));
    }

    // elem * d = depile(p);
    // while (d) {
    //     int son_fd = open(d -> path -> data,O_RDONLY);
    //     i = oe(son_fd,i,d -> path,cmd);
    //     elem * f = d;
    //     d = depile(p);
    //     string_delete(f -> path);
    //     free(f);
    // }

    // free(p);
    string_delete(s);
    closedir(dir);
    return i;
}



int main (int argc, char ** argv) {
    int fd = open(".",O_RDONLY);
    char ** cmd = malloc(sizeof(char *) * 4096);
    for (int i = 0; i< 4096; i++) {
      cmd[i] = malloc(sizeof(char) * 4096);
    }
    struct string * path = string_new(PATH_MAX);
    string_append(path, ".");
    int j = oe(fd,1,path,cmd);
    cmd[0] = "ls";
    cmd[j] = NULL;
    cmd = realloc(cmd,(j+1)*sizeof(char *));
    for(int i = 0 ; i < j; i++) {
      printf("%s \n",cmd[i]);
    }
    execvp("ls",cmd);
    perror("exec");
    return 0;
}