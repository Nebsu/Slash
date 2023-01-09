#define  _BSD_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/dir.h>

#define CHAR_SKIP_1 1
#define CHAR_SKIP_2 4
#define SLASH "/"

char OLD_PWD [PATH_MAX];
char five_chars[5];
char bufferPoint[PATH_MAX];
char tmpPath [PATH_MAX];

/**
 *  Fonction qui permet de 
*/

int remove_rep(char *path, int len, int totalLength, int charskipped, int end) {
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
        if(!end) memmove(&path[i+1],&path[len+1],totalLength-len);
        else path[i + 1] = '\0';
    }
    return len - i ;
}

/**
 * Fonction permettant de simplifer l'écriture d'un chemin en retirant et 
 * en interprétant ses ./ et ../
*/
char * path_simplificator (char * path,size_t len) {
    int i = 0;
    int point = 0;
    int del;
    while (i < len) {
        if (path[i] == '/') {
            if (point == 2) {
                del = remove_rep(path,i,len,CHAR_SKIP_2,0);
                len -= del;
                i -= del;
                del = 0;
            }
            else if (point == 1) {
                del = remove_rep(path,i,len,CHAR_SKIP_1,0);
                len -= del;
                i -= del;
                del = 0;
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
        len -= remove_rep(path, i, len, CHAR_SKIP_2, 1);
    }
    else if (point == 1) {
        len -= remove_rep(path, i, len, CHAR_SKIP_1, 1);
    }
    return path;
}

char buffer_path [PATH_MAX];
char tmpEnv [PATH_MAX];

int f (char * path) {
    int i = 0;
    while( path[i] != '\0') {
        if (path[i] == '/') {
            return i;
        }
        i++;
    }
    return i;
}

char * setDirectory(char * path) {
    if (path[0] == '/') {
        strcpy(tmpEnv,path);
    }
    else {
        char *tmp = malloc(PATH_MAX);
        if (!tmp) { 
            perror("malloc cd");
            return NULL;
        }
        strcpy(tmpEnv, getenv("PWD"));
        strcat(tmpEnv, SLASH);
        strcpy(tmp, path);
        strcat(tmpEnv, tmp);
        free(tmp);
    }
    path_simplificator(tmpEnv, strlen(tmpEnv));
    return tmpEnv;
}


int change_dir(char * path,int physical) {
    int d = -1;
    int moins = 0;
    int  home = 0;
    strcpy(OLD_PWD, getenv("PWD"));
    if (strcmp(path, "") == 0) {
        d = chdir(getenv("HOME"));  
        strcpy(buffer_path,getenv("HOME"));
        home = 1;
    }
    else if (strcmp(path, "-") == 0) {
        moins = 1;
        if (getenv("OLDPWD") != NULL) {
            d = chdir(getenv("OLDPWD"));
            strcpy(buffer_path,getenv("OLDPWD"));
        }
        else printf("Pas de répertoire ancien");
    } 
    else {
       if (physical) {
            getcwd(buffer_path,PATH_MAX);
            d = chdir(buffer_path);
            if(d == -1) {
                perror("chdir");
            }
            d = chdir(path);

        }
        else {
            strcpy(buffer_path,setDirectory(path));
            d = chdir(buffer_path);
        }
        if (d == -1) {
            d = chdir(path);
            physical = 1;
        }
    }
    if (d == -1) {
        perror("chdir");
        return 1;
    }
    strncpy(five_chars, path, 5);
    if (physical || strcmp(five_chars,"../..") == 0) {
        getcwd(buffer_path,PATH_MAX);
    }
    else if( !moins && !home) {
        strcpy(buffer_path,setDirectory(path));
    }

    if(strlen(buffer_path) > 1 && buffer_path[strlen(buffer_path) - 1] == '/') buffer_path[strlen(buffer_path) - 1] = '\0';
    setenv("PWD", buffer_path, 1);
    setenv("OLDPWD", OLD_PWD, 1);
    chdir(getenv("PWD"));
    return 0;
}


int cd (int argc, char ** argv) {
    switch (argc) {
            case 1 :
                return change_dir("", 0);
            case 2 :
                if (argv[1][0] == '-' && strlen(argv[1]) > 1)
                {
                    if (strcmp(argv[1], "-P") == 0) {
                        return change_dir(argv[2], 1);
                    }
                    else if (strcmp(argv[1], "-L") == 0) {
                        return change_dir(argv[2], 0);
                    }
                    else {
                        printf("Mauvais format ,format attendu : cd [-L | -P] [ref | -] \n");
                        return 1;
                    }
                }
                else {
                    return change_dir(argv[1], 0);
                }
            case 3 :
                if (strcmp(argv[1], "-P") == 0) {
                    return change_dir(argv[2], 1);
                }
                else if (strcmp(argv[1], "-L") == 0) {
                    return change_dir(argv[2], 0);
                }
                else {
                    printf("Mauvais format ,format attendu : cd [-L | -P] [ref | -] \n");
                    return 1;
                }
        }
    return 0;
}
