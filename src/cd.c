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

#define SLASH "/"

char OLD_PWD [PATH_MAX];


int remove_rep(char *path,int len,int totalLength,int charskipped) {
    int i = len - charskipped;
    while(i > 0 && path[i] != '/') {
        i--;
    }
    if(i <= 0) {
        path[0] = '/';
        strcpy(&path[0],path + len );
        i = 0;
    }
    else {
        memmove(&path[i+1],&path[len+1],totalLength-len);
    }
    return len - i ;
}


char * path_simplificator (char * path,size_t len) {
    int i = 0;
    int point = 0;
    int oldlen = len;
    int del;
    while (i < len) {
        if (path[i] == '/') {
            if (point == 2) {
                del = remove_rep(path,i,len,CHAR_SKIP_2);
                len -= del;
                i -= del;
                del=0;
            }
            else if (point == 1) {
                del = remove_rep(path,i,len,CHAR_SKIP_1);
                len -= del;
                i -= del;
                del=0;
            }
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
        remove_rep(path,i,len,CHAR_SKIP_2);
    }
    else if (point == 1) {
        remove_rep(path,i,len,CHAR_SKIP_1);
    }
    for(int j = len; j < oldlen;j++) {
        path[j] = '\0';
    }
    return path;
}

char buffer [PATH_MAX];
char tmpEnv [PATH_MAX];

char * setDirectory(char * path) {
    if (path[0] == '/') {
        strcpy(tmpEnv,path);
    }
    else {
        char *tmp = malloc(4098);
        if (!tmp) { 
            perror("malloc cd");
            return NULL;
        }
        strcpy(tmpEnv,getenv("PWD"));
        strcat(tmpEnv,SLASH);
        strcpy(tmp,path);
        strcat(tmpEnv,tmp);

        free(tmp);
    }
    path_simplificator(tmpEnv,strlen(tmpEnv));
    return tmpEnv;
}


int change_dir(char * path,int physical) {
    int d = -1;
    int moins = 0;
    int  home = 0;
    strcpy(OLD_PWD,getenv("PWD"));
    if (strcmp(path,"") == 0) {
        d = chdir(getenv("HOME"));  
        strcpy(buffer,getenv("HOME"));
        home = 1;
    }
    else if (strcmp(path,"-") == 0) {
        moins = 1;
        if (getenv("OLDPWD") != NULL) {
            d = chdir(getenv("OLDPWD"));
            getcwd(buffer,PATH_MAX);
            printf("new path %s \n",buffer);
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
    if (physical) {
        getcwd(buffer,PATH_MAX);
    }
    else if( !moins && !home){
        strcpy(buffer,setDirectory(path));
    }
    if(strlen(buffer) > 1 && buffer[strlen(buffer)- 1] == '/') buffer[strlen(buffer) - 1] = '\0';
    setenv("PWD",buffer,1);
    setenv("OLDPWD",OLD_PWD,1);
    return 0;
}

int cd (int argc, char ** argv) {

    switch (argc) {
            case 1 : return change_dir("",0);break;
            case 2 : if(argv[1][0] == '-' && strlen(argv[1]) > 1) {
                 if (strcmp(argv[1],"-P") == 0) {
                return change_dir(argv[2],1);
            }
            else if (strcmp(argv[1],"-L") == 0) {
                return change_dir(argv[2],0);
            }
            else {
                printf("Mauvais format ,format attendu : cd [-L | -P] [ref | -] \n");
                return 1;
            }break;
            }
            return change_dir(argv[1],0);break;
            case 3 : 
            if (strcmp(argv[1],"-P") == 0) {
                return change_dir(argv[2],1);
            }
            else if (strcmp(argv[1],"-L") == 0) {
                return change_dir(argv[2],0);
            }
            else {
                printf("Mauvais format ,format attendu : cd [-L | -P] [ref | -] \n");
                return 1;
            }break;
                
        }
    return 0;
}


int main(int argc, char ** argv) {
    if (argc > 3) return -1;
    cd(argc,argv);
    printf("OLDPWD = %s\n",getenv("OLDPWD"));
    printf("nPWD = %s\n",getenv("PWD"));
    // return cd(argv[1]);
}
