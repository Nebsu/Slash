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
#include "star.h"

int isFormat(char * str, char * regEx) {
    int strLen = strlen(str);
    int regLen = strlen(regEx);
    int idxStr = strLen-1;
    int idxReg = regLen-1;
    if(regEx[idxReg] == '/') {
        idxReg--;
    }
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
    if(idxReg == 0 && regEx[0] == '*') {
        return 1;
    }
    return idxStr == -1 && idxReg == -1;
}

char** cut(char * path) {
    char ** res = malloc(sizeof(char *)*2);
    int i = 0;
    while(i < strlen(path) && path[i] != '*') {
        i++;
    }
    if(i == strlen(path)) {
        res[0] = malloc(sizeof(char) + 1);
        res[0][0] = '.';
        res[0][1] = '\0';
        res[1] = malloc(sizeof(char) * strlen(path) + 1);
        strcpy(res[1],path);
        return res;
    }
    while(i > 0 && path[i] != '/') {
        i--;
    }
    if(i == 0) {
        res[0] = malloc(sizeof(char) + 1);
        res[0][0] = '.';
        res[0][1] = '\0';
        res[1] = malloc(sizeof(char) * strlen(path) + 1);
        strcpy(res[1],path);
        return res;
    }
    res[0] = malloc(sizeof(char)*i + 2);
    res[1] = malloc(sizeof(char)*(strlen(path)-i) + 1);
    strncpy(res[0],path,i + 1);
    res[0][i+1] = '\0';
    strncpy(res[1],path+i+1,strlen(path)-i);
    res[1][strlen(path)-i] = '\0';
    return res;
}

char * cutRegEx(char * regEx) {
    int i = 0;
    while(i < strlen(regEx) && regEx[i] != '/') {
        i++;
    }
    char * res = malloc(sizeof(char) * (strlen(regEx) - i) + 1);
    strncpy(res,regEx+i+1,strlen(regEx)-i);
    res[strlen(regEx)-i] = '\0';
    return res;
}

int traitementAsFile(char * regEx) {
    int i = 0;
    while(i < strlen(regEx) && regEx[i] != '/') {
        i++;
    }
    if(i == strlen(regEx)) {
        return 1;
    }
    if(i == strlen(regEx) - 1 && regEx[i] == '/') {
        return 1;
    }
    return 0;
}

int getFiles(char * path, char ** buf, char * regEx, int i) {
    // printf("path = %s regEx = %s\n",path,regEx);
    DIR * dir;
    struct dirent * ent;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if(ent->d_name[0] == '.') continue;
            if(isFormat(ent->d_name,regEx)) {
                // printf("file = %s\n",ent->d_name);
                if(ent->d_type == DT_DIR || ent->d_type == DT_LNK) {
                    if(traitementAsFile(regEx)) {
                        if(strlen(path) == 1 && path[0] == '.') {
                            buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + 1);
                            strcpy(buf[i],ent->d_name);
                        }
                        else if (strlen(path) == 2 && path[0] == '.' && path[1] == '/') {
                            buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + 3);
                            strcpy(buf[i],"./");
                            strcat(buf[i],ent->d_name);
                        }
                        else {
                            if(path[strlen(path)-1] == '/') {
                                buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + strlen(path) + 1);
                                strcpy(buf[i],path);
                                strcat(buf[i],ent->d_name);
                            }
                            else {
                                buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + strlen(path) + 2);
                                strcpy(buf[i],path);
                                strcat(buf[i],"/");
                                strcat(buf[i],ent->d_name);
                            }
                        }
                        i++;
                    }
                    else {
                        if(strlen(path) > 1 &&((path[strlen(path) - 2] == '.' && path[strlen(path) - 1] == '.') || (path[strlen(path) - 2] == '.' && path[strlen(path) - 1] == '/'))) {
                            char * newPath = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                            strcpy(newPath,path);
                            strcat(newPath,"/");
                            strcat(newPath,ent->d_name);
                            char * newRegEx = cutRegEx(regEx);
                            i = getFiles(newPath,buf,newRegEx,i);
                            free(newPath);
                            free(newRegEx);
                        }
                        else if(strlen(path) == 1 && path[0] == '.') {
                            char * newPath = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                            strncpy(newPath,path,strlen(path) - 1);
                            strcat(newPath,ent->d_name);
                            char * newRegEx = cutRegEx(regEx);
                            i = getFiles(newPath,buf,newRegEx,i);
                            free(newPath);
                            free(newRegEx);
                        }
                        else {
                            char * newPath = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                            strcpy(newPath,path);
                            strcat(newPath,"/");
                            strcat(newPath,ent->d_name);
                            char * newRegEx = cutRegEx(regEx);
                            i = getFiles(newPath,buf,newRegEx,i);
                            free(newPath);
                            free(newRegEx);
                        }
                    }
                }
                else if (ent->d_type == DT_REG) {
                    if(strlen(path) == 1 && path[0] == '.') {
                        buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + 1);
                        strcpy(buf[i],ent->d_name);
                    }
                    else if (strlen(path) == 2 && path[0] == '.' && path[1] == '/') {
                        buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + 3);
                        strcpy(buf[i],"./");
                        strcat(buf[i],ent->d_name);
                    }
                    else {
                        if(path[strlen(path)-1] == '/') {
                            buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + strlen(path) + 1);
                            strcpy(buf[i],path);
                            strcat(buf[i],ent->d_name);
                        }
                        else {
                            buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + strlen(path) + 2);
                            strcpy(buf[i],path);
                            strcat(buf[i],"/");
                            strcat(buf[i],ent->d_name);
                        }
                    }
                    i++;
                }
            }
        }
        closedir(dir);
    }
    buf[i] = NULL;
    return i;
}

char** star(int argc, char ** argv) {
    char ** buf = malloc(sizeof(char *) * 4096);
    buf[0] = argv[0];
    int j = 1;
    for(int i = 1; i < argc; i++) {
        if(strchr(argv[i], '*') != NULL) {
            char **tmp = cut(argv[i]);
            char * path = tmp[0];
            char * regEx = tmp[1];
            j = getFiles(path,buf,regEx,j);
        }
        else {
            buf[j++] = argv[i];
        }
    }
    return buf;
}

