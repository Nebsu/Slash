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

//tmp

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
            if (isFormat(ent->d_name,regEx) || strcmp(regEx,"") == 0) {
                // printf("Lecture de : %s\n", ent->d_name);
                if(ent->d_type == DT_DIR) {
                    char * tmp = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
                    strcpy(tmp,path);
                    strcat(tmp,"/");
                    strcat(tmp,ent->d_name);
                    int tailleReg = 0;
                    while (regEx[tailleReg] != '/' && regEx[tailleReg] != '\0') {
                        tailleReg++;
                    }
                    // printf("1regEx = %s tailleReg = %d fichier = %s\n",regEx,tailleReg, ent->d_name);
                    if(tailleReg != 0) {
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
                        i = getFiles(tmp, buf, regEx + tailleReg, i);
                    }
                }
                else {
                    //faire detetcion fin
                    int tailleReg = 0;
                    while (regEx[tailleReg] != '/' && regEx[tailleReg] != '\0') {
                        tailleReg++;
                    }
                    // printf("2regEx = %s tailleReg = %d fichier = %s\n",regEx,tailleReg, ent->d_name);
                    int regexLen = cut(regEx);
                    if(regexLen == strlen(regEx)) {
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
    if(strchr(tmp, '*') != NULL) {
        // printf("tmp = .\n");
        // printf("path = %s\n",tmp);
        getFiles(".",buf,tmp,0);
    }
    else {
        // printf("tmp = %s\n",tmp);
        // printf("path = %s\n",path+i);
        getFiles(tmp, buf, path+i, 0);
    }

    if(buf[0] == NULL) {
        // printf("Aucun fichier trouvé\n");
    }
    else {
        execvp(argv[0],buf);
    }
    return 127;
}

