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

char buff [4096] [4096];

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

int cut(char * path) {
    int i = 0;
    while (path[i] != '\0') {
        i++;
        if (path[i] == '*') {
            return i;
        }
    }
    return i;
}

int getFiles(char * path, char ** buf, char * regEx, int i) {
    if(strlen(path) == 0) {
        path = ".";
    }
    // printf("path = %s\n",path);
    // printf("regEx = %s\n",regEx);
    DIR * dir;
    struct dirent * ent;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            // printf("Lecture de : %s\n", ent->d_name);
            if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0 || ent->d_name[0] == '.')continue;
            char * sousRegex = malloc(sizeof(char) * (strlen(regEx) + 1));
            int idx = cut(regEx);
            strncpy(sousRegex,regEx,idx);
            sousRegex[idx] = '\0';
            // printf("sousRegex = %s\n",sousRegex);
            if (isFormat(ent->d_name,sousRegex) || strcmp(sousRegex,"") == 0) {
                // printf("idx = %d\n",idx);
                // printf("Lecture de : %s\n", ent->d_name);
                if(ent->d_type == DT_DIR) {
                    // printf("1regEx = %s tailleReg = %d dossier = %s\n",regEx,tailleReg, ent->d_name);
                    if(strlen(regEx) == strlen(sousRegex)) {
                        buf[i] = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                        if(strcmp(path,".") == 0) {
                            strcpy(buf[i],ent->d_name);
                        }
                        else {
                            strcpy(buf[i],path);
                            strcat(buf[i],"/");
                            strcat(buf[i],ent->d_name);
                        }
                        i++;
                    }
                    else {
                        char * tmp = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                        if(strcmp(path,".") == 0) {
                            strcpy(tmp,ent->d_name);
                        }
                        else {
                            strcpy(tmp,path);
                            strcat(tmp,"/");
                            strcat(tmp,ent->d_name);
                        }
                        i = getFiles(tmp, buf, regEx + idx, i);
                    }
                }
                else {
                    // printf("2regEx = %s fichier = %s\n",regEx, ent->d_name);
                    if(strlen(regEx) == strlen(sousRegex)) {
                        buf[i] = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                        if(strcmp(path,".") == 0) {
                            strcpy(buf[i],ent->d_name);
                        }
                        else {
                            strcpy(buf[i],path);
                            strcat(buf[i],"/");
                            strcat(buf[i],ent->d_name);
                        }
                        i++;
                    }
                }
            }
            else {
                // printf("non format = %s\n",ent->d_name);
            }
        }
    }
    return i;
}


// Lecture -> Repertoire courant -> Si le nom respecte le format -> Ajout dans la liste -> Si il y a un "/" 
// on ajoute pas les fichiers ordinaires



int star(int argc, char ** argv) {
    char * path = malloc(sizeof(char) * 4096);
    path = argv[1];
    char * tmp = malloc(sizeof(char) * 4096);
    int i = cut(path);
    strncpy(tmp,path, i);
    char ** buf = malloc(sizeof(char *) * 4096);
    getFiles(".",buf,path,1);
    buf[0] = argv[0];
    execvp(argv[0],buf);
    return 127;
}

