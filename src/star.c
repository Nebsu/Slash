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

#include "star.h"
#include "cd.h"

int nbc = 0;

int dirNeed(char * regEx){
    int i = 0;
    while (regEx[i] != '/' && regEx[i] != '\0') {
        i++;
    }
    return i != strlen(regEx);
}

void nettoyageSlash(char * regEx){
    int i = 0;
    while (regEx[i] != '\0') {
        if (regEx[i] == '/' && regEx[i+1] == '/') {
            memmove(regEx+i, regEx+i+1, strlen(regEx+i+1)+1);
            i--;
        }
        i++;
    }
}


int isFormat(char * str, char * regEx) {
    //recupérer le repertoire le plus a gauche de str et le stocker dans un char * tmp
    int i = 0;
    while (regEx[i] != '/' && regEx[i] != '\0') {
        i++;
    }
    char * tmp = malloc(i + 1);
    strncpy(tmp, regEx, i);
    tmp[i] = '\0';
    int strLen = strlen(str);
    int regLen = strlen(tmp);
    int idxStr = strLen-1;
    int idxReg = regLen-1;
    if(tmp[idxReg] == '/') {
        idxReg--;
    }
    while ( idxStr >= 0 && idxReg >= 0 ) {
        if (tmp[idxReg] == '*') {
            free(tmp);
            return 1;
        }
        if (str[idxStr] != tmp[idxReg] ) {
            free(tmp);
            return 0;
        }
        else {
            idxStr--;
            idxReg--;
        }
    }
    if(idxReg == 0 && tmp[0] == '*') {
        free(tmp);
        return 1;
    }
    free(tmp);
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
    if((strchr(regEx,'/') == NULL) || ((strchr(regEx,'/') != NULL) && strlen(strchr(regEx,'/')) == 1)) {
        char * res = malloc(sizeof(char) * strlen(regEx) + 1);
        strcpy(res,regEx);
        return res;
    }
    if(strstr(regEx, "**") != NULL) {
        int i = 3;
        while(regEx[i] != '/') {
            i++;
        }
        char * res = malloc(sizeof(char) * i + 1);
        strncpy(res, regEx, 3);
        strncat(res, regEx + i, strlen(regEx) - i);
        return res;
    }
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
    if(i == strlen(regEx) || (i == strlen(regEx) - 1 && regEx[i] == '/')) {
        return 1;
    }
    return 0;
}

char troisChar[4];
char deuxChar2[3];

int getFiles(char * path, char ** buf, char * regEx, int i, int doubleEtoile) {
    // printf("regEx: %s\n", regEx);
    DIR * dir;
    struct dirent * ent;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            troisChar[3] = '\0';
            deuxChar2[2] = '\0';
            strncpy(troisChar,regEx,3);
            strncpy(deuxChar2,regEx,2);
            if(ent->d_name[0] == '.' && !strcmp(troisChar,"../") == 0 && !strcmp(deuxChar2,"./") == 0 )continue;
            if(isFormat(ent->d_name,regEx) || (!dirNeed(regEx) && ent -> d_type == DT_REG && strlen(regEx) > 3 && isFormat(ent -> d_name,regEx + 3)) || (doubleEtoile && ((ent->d_type == DT_DIR || ent->d_type == DT_LNK)))) {
                if(doubleEtoile && (((ent->d_type == DT_REG) && !dirNeed(regEx)) || ((ent->d_type == DT_DIR || ent->d_type == DT_LNK) && dirNeed(regEx)))) {
                    if(strchr(regEx,'/') != NULL) {
                        if(strcmp(regEx,"**/") == 0) {
                            if(strcmp(path,".") == 0) {
                                buf[i] = malloc(sizeof(char) * (strlen(ent->d_name) + 2));
                                strcpy(buf[i],ent->d_name);
                            }
                            else {
                                buf[i] = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 3));
                                strcpy(buf[i],path);
                                strcat(buf[i],"/");
                                strcat(buf[i],ent->d_name);
                            }
                            strcat(buf[i],"/");
                            i++;
                            if(strcmp(path,".") == 0) {
                                char * newPath = malloc(sizeof(char) * (strlen(ent->d_name) + 1));
                                strcpy(newPath,ent->d_name);
                                char * newRegEx = cutRegEx(regEx);
                                i = getFiles(newPath,buf,newRegEx,i,doubleEtoile);
                                free(newPath);
                                free(newRegEx);
                            }
                            else {
                                char * newPath = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                                strcpy(newPath,path);
                                strcat(newPath,"/");
                                strcat(newPath,ent->d_name);
                                char * newRegEx = cutRegEx(regEx);
                                i = getFiles(newPath,buf,newRegEx,i,doubleEtoile);
                                free(newRegEx);
                                free(newPath);
                            }
                        }   
                        else {
                            if(strcmp(path,".") == 0) {
                                char * newPath = malloc(sizeof(char) * (strlen(ent->d_name) + 1));
                                strcpy(newPath,ent->d_name);
                                char * newRegEx = cutRegEx(regEx + 3);
                                if (isFormat(ent->d_name,regEx + 3)) i = getFiles(newPath,buf,newRegEx,i,0);
                                if(ent->d_type != DT_LNK) {
                                    i = getFiles(newPath,buf,regEx,i,doubleEtoile);
                                }
                                free(newRegEx);
                                free(newPath);
                            }
                            else {
                                char * newPath = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                                strcpy(newPath,path);
                                strcat(newPath,"/");
                                strcat(newPath,ent->d_name);
                                // printf("cut dn %s -> regEx %s \n ",ent -> d_name,regEx);
                                char * newRegEx = cutRegEx(regEx + 3);
                                // i = getFiles(newPath,buf,newRegEx,i,0);
                                if (isFormat(ent->d_name,regEx + 3) || !dirNeed(regEx)) i = getFiles(newPath,buf,newRegEx,i,0);
                                if(ent->d_type != DT_LNK ) {
                                    i = getFiles(newPath,buf,regEx,i,doubleEtoile);
                                }
                                free(newRegEx);
                                free(newPath);
                            }
                        }
                    }
                    else if(strchr(regEx,'/') == NULL) {
                        if(strcmp(path,".") == 0) {
                            buf[i] = malloc(sizeof(char) * (strlen(ent->d_name) + 1));
                            strcpy(buf[i],ent->d_name);
                        }
                        else {
                            buf[i] = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                            strcpy(buf[i],path);
                            strcat(buf[i],"/");
                            strcat(buf[i],ent->d_name);
                        }
                        i++;
                    }
                }
                else if((ent->d_type == DT_DIR || ent->d_type == DT_LNK) && (!traitementAsFile(regEx))) {
                    char * newPath;
                    if(strlen(path) > 1 &&((path[strlen(path) - 2] == '.' && path[strlen(path) - 1] == '.') || (path[strlen(path) - 2] == '.' && path[strlen(path) - 1] == '/'))) {
                        newPath = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                        strcpy(newPath,path);
                        strcat(newPath,ent->d_name);
                    }
                    else if(strlen(path) == 1 && path[0] == '.') {
                        newPath = malloc(sizeof(char) * strlen(ent->d_name) + 1);
                        strcpy(newPath,ent->d_name);
                    }
                    else {
                        if(path[strlen(path)-1] == '/' ) {
                            newPath = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 1));
                            strcpy(newPath,path);
                            strcat(newPath,ent->d_name);
                            if (strcmp(troisChar,"../") == 0 || strcmp(deuxChar2,"./") == 0) strcat(newPath,"/");
                        }
                        else {
                            newPath = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 3));
                            strcpy(newPath,path);
                            strcat(newPath,"/");
                            strcat(newPath,ent->d_name);
                            if (strcmp(troisChar,"../") == 0 || strcmp(deuxChar2,"./") == 0) strcat(newPath,"/");
                        }
                    }
                    char * newRegEx = cutRegEx(regEx);
                    i = getFiles(newPath,buf,newRegEx,i,doubleEtoile);
                    free(newPath);
                    free(newRegEx);
                }
                else if (((ent->d_type == DT_REG) && !dirNeed(regEx)) || ((ent->d_type == DT_DIR || ent->d_type == DT_LNK) && (traitementAsFile(regEx)))) {
                    if(strlen(path) == 1 && path[0] == '.') {
                        buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + 2);
                    }
                    else if (strlen(path) == 2 && path[0] == '.' && path[1] == '/') {
                        buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + 3);
                        strcpy(buf[i],"./");
                    }
                    else {
                        if(path[strlen(path)-1] == '/') {
                            buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + strlen(path) + 1);
                            strcpy(buf[i],path);
                        }
                        else {
                            buf[i] = malloc(sizeof(char) * strlen(ent->d_name) + strlen(path) + 2);
                            strcpy(buf[i],path);
                            strcat(buf[i],"/");
                        }
                    }
                    if(strlen(path) == 1 && path[0] == '.') strcpy(buf[i],ent -> d_name);
                    else strcat(buf[i],ent->d_name);
                    i++;
                }
            }
        }
        closedir(dir);
    }
    buf[i] = NULL;
    return i;
}

char * add_star_slash(char * str) {
    char * tmp = malloc(strlen(str) + 3);
    strncpy(tmp,str,3);
    tmp[3] = '\0';
    strcpy(tmp+3,"*/");
    strcpy(tmp+5,str+3);
    tmp[strlen(str) + 2] = '\0';
    free(str);
    return tmp;
}

char** star(int argc, char ** argv) {
    char ** buf = malloc(sizeof(char *) * 4096);
    int j = 0;
    for(int i = 0; i < argc; i++) {
        if(strchr(argv[i], '*') != NULL) {
            nettoyageSlash(argv[i]);
            char **tmp = cut(argv[i]);
            char * path = tmp[0];
            char * regEx = tmp[1];
            if(strstr(regEx,"**") != NULL && strlen(regEx) > 3 && !dirNeed(regEx+3)) {
                char * tmp = add_star_slash(regEx);
                regEx = tmp;
            }
            int tmp2 = getFiles(path,buf,regEx,j,strstr(argv[i],"**") != NULL);
            if(tmp2 == j) {
                buf[j] = malloc(sizeof(char *) + 1);
                strcpy(buf[j],argv[i]);
                j++;
            }
            else {
                j = tmp2;
            }
            free(tmp[0]);
            free(regEx);
            free(tmp);
        }
        else {
            buf[j] = malloc(strlen(argv[i])+ 1);
            strcpy(buf[j],argv[i]);
            j++;
        }
            free(argv[i]);
    }
    buf[j] = NULL;
    nbc = j;
    return buf;
}