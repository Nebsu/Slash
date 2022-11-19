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

#define CHAR_SKIP_1 1

#define CHAR_SKIP_2 4

int remove_rep(char *path,int len,int charskipped) {
    int i = len - charskipped;
    while(i > 0 && path[i] != '/') {
        printf(" i =  %d ,char = %c \n",i,path[i]);
        i--;
    }
    printf("partie à enlever : ");
    for (int j = i;j < len;j++) {
        printf("%c",path[j]);
    }
    printf("\n");
    if(i <= 0) {
        printf(" i =  %d ,char = %c \n",i,path[i]);
        path[0] = '/';
        strcpy(&path[0],path + len );
        i = 0;
    }
    else {
        strcpy(&path[i+1],path + len + 1 );
    }
    return len - i ;
}


char * path_simplificator (char * path,size_t len) {
    int i = 0;
    int point = 0;
    int del;
    while (i < len) {
        if (path[i] == '/') {
            if (point == 2) {
                del = remove_rep(path,i,CHAR_SKIP_2);
                len -= del;
                i -= del;
                del=0;
                printf("newpath = %s \n",path);
            }
            else if (point == 1) {
                del = remove_rep(path,i,CHAR_SKIP_1);
                len -= del;
                i -= del;
                del=0;
            point = 0;
        }
        else if(path[i] == '.') {
            if (point > 1) {
                return NULL;
            }
            point++;

        }
        else {
            point = 0;
        }
        i++;
    }
    if (point == 2) {
        remove_rep(path,i,CHAR_SKIP_2);
    }
    else if (point == 1) {
        remove_rep(path,i,CHAR_SKIP_1);
    }
    return path;
}

char buffer [PATH_MAX];
int cd_logical (char * path,int physical) {
    int d = -1;
    if (strcmp(path,"") == 0) {
        d = chdir(getenv("HOME"));  
    }
    else if (strcmp(path,"-") == 0) {
        if (getenv("OLDPWD") != NULL) {
            d = chdir(getenv("OLDPWD"));
            // prientf(" PWD = %s\n",getenv("PWD"));

        }
        else printf("Pas de répertoire ancien");
    }
    else {
        d = chdir(path);
    }
    if (d == -1) {
        perror("chdir");
        return 1;
    }

    if(physical) {
        getcwd(buffer,PATH_MAX); 
    }
    else {
        if (path[0] == '/' ) {
        //path_simplificator(path)
        }
        else {
            //path_simplificator
        }
    }
    setenv("PWD",buffer,1);
    // printf(" PWD = %s\n",getenv("PWD"));
    return 0;
}


int main(int argc, char ** argv) {
    if (argc > 3) return -1;
    // if (argc == 1) return cd("");
    // Cas à faire pour "cd vide
    //m
    // printf("PWD = %s\n",getenv("PWD"));
    path_simplificator(argv[1],strlen(argv[1]));
    printf("nPath = %s \n",argv[1]);
    // return cd(argv[1]);
}
