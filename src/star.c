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
char * pp = "..";

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

int isDirectory(char ** pathlist, char * path, int depth) {
    return path[strlen(pathlist[depth])] == '/' ? 0 : 1;
}

char ** getPathSplit(char * path) {
    char ** pathSplit = malloc(sizeof(char *) * 4096);
    int i = 0;
    char * token = strtok(path, "/");
    while (token != NULL) {
        pathSplit[i] = malloc(sizeof(char) * 4096);
        strcpy(pathSplit[i], token);
        i++;
        token = strtok(NULL, "/");
    }
    pathSplit[i] = NULL;
    return pathSplit;
}

char ** getFiles(char * path, char ** regExlist, char * list, char * command, int isDir, int depth) {
    DIR * dir;
    struct dirent * ent;
    // Tableau de String retour
    char ** files = malloc(sizeof(char *) * 4096);
    // Stockage du parcours
    char ** newFiles = malloc(sizeof(char *) * 4096);
    files[0] = malloc(sizeof(char) * 4096);
    // Copie du chemin actuel pour pouvoir le modifier
    char * pathcpy = malloc(sizeof(char) * 4096);
    // Indice du tableau de String
    int nbFiles = 1;
    // Taille de regExlist
    int regSize = 0;
    // Boolean pour savoir le prochain sera un fichier ou un dossier
    int isD = 1;
    int i = 0;

    printf("actualpath = %s\n", path);
    // Récupération de la taille de regExlist
    while (regExlist[regSize] != NULL) {
        regSize++;
    }
    // Si le chemin est un dossier
    if(depth < regSize-1) {
        isD = isDirectory(regExlist, list, depth+1);
    }
    printf("isD = %d\n", isD);
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0 || ent->d_name[0] == '.')continue;
            if (isFormat(ent->d_name,regExlist[depth])) {
                // Si c'est un dossier et qu'il n y a pas de / a la fin de la regex
                if (ent->d_type == DT_DIR && isDir == 0) {
                    files = realloc(files, sizeof(char *) * (nbFiles + 2));
                    files[nbFiles] = malloc(sizeof(char) * (strlen(ent->d_name) + 1));
                    strcpy(files[nbFiles],ent->d_name);
                    strcat(files[nbFiles],"/");
                    strcpy(pathcpy, path);
                    strcat(pathcpy,files[nbFiles]);
                    printf("path = %s\n",pathcpy);
                    //TODO : La recursion
                    // if(depth < regSize-1) {
                    //     newFiles = getFiles(pathcpy, regExlist, list, command, isDir, depth+1);
                    // }
                    nbFiles++;
                } 
                // Si c'est un fichier et qu'il n y a pas de / a la fin de la regex
                else if (isDir == 1) {
                    files = realloc(files, sizeof(char *) * (nbFiles + 2));
                    files[nbFiles] = malloc(sizeof(char) * (4096));
                    strcpy(files[nbFiles],path);
                    strcat(files[nbFiles],ent->d_name);
                    nbFiles++;
                }
                while (newFiles[i] != NULL) {
                    files = realloc(files, sizeof(char *) * (nbFiles + 1));
                    files[nbFiles] = malloc(sizeof(char) * (strlen(newFiles[i]) + 1));
                    strcpy(files[nbFiles],newFiles[i]);
                    nbFiles++;
                    i++;
                }
            }
        }
        closedir(dir);
    }
    else {
        perror("opendir");
    }
    return files;
}

// Lecture -> Repertoire courant -> Si le nom respecte le format -> Ajout dans la liste -> Si il y a un "/" 
// on ajoute pas les fichiers ordinaires

// char ** getFilesRec(char * path, char * regEx) {
//     DIR * dir;
//     struct dirent * ent;
//     char ** files = NULL;
//     int nbFiles = 0;
//     if ((dir = opendir(path)) != NULL) {
//         while ((ent = readdir(dir)) != NULL) {
//             if (isFormat(ent->d_name,regEx)) {
//                 files = realloc(files, sizeof(char *) * (nbFiles + 1));
//                 files[nbFiles] = malloc(sizeof(char) * (strlen(ent->d_name) + 1));
//                 strcpy(files[nbFiles],ent->d_name);
//                 nbFiles++;
//             }
//             if (ent->d_type == DT_DIR && strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0) {
//                 char * newPath = malloc(sizeof(char) * (strlen(path) + strlen(ent->d_name) + 2));
//                 strcpy(newPath,path);
//                 strcat(newPath,"/");
//                 strcat(newPath,ent->d_name);
//                 char ** newFiles = getFilesRec(newPath,regEx);
//                 int i = 0;
//                 while (newFiles[i] != NULL) {
//                     files = realloc(files, sizeof(char *) * (nbFiles + 1));
//                     files[nbFiles] = malloc(sizeof(char) * (strlen(newFiles[i]) + 1));
//                     strcpy(files[nbFiles],newFiles[i]);
//                     nbFiles++;
//                     i++;
//                 }
//             }
//         }
//         closedir(dir);
//     }
//     else {
//         perror("opendir");
//     }
//     return files;
// }


int main(int argc, char const *argv[])
{
    char * l = "*/*";
    char * l2 = malloc(sizeof(char) * 4096);
    strcpy(l2,l);
    char ** files = getPathSplit(l2);
    char * path = malloc(sizeof(char) * 4096);
    strcat(path,"./");

    char ** files2 = getFiles(path, files, l,"ls", isDirectory(files, l, 0), 0);

    char ** res = malloc(sizeof(char *) * 4096);
    int i = 0;
    while (files2[i] != NULL) {
        printf("%s\n",files2[i]);
        i++;
    }
    execvp("ls",files2);

    return 0;
}

