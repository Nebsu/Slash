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

char ** getFiles(char * path, char * regEx, char * command) {
    DIR * dir;
    struct dirent * ent;
    char ** files = malloc(sizeof(char *) * 4096);
    files[0] = malloc(sizeof(char) * 4096);
    int nbFiles = 1;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)continue;
            if (isFormat(ent->d_name,regEx)) {
                files = realloc(files, sizeof(char *) * (nbFiles + 2));
                files[nbFiles] = malloc(sizeof(char) * (strlen(ent->d_name) + 1));
                strcpy(files[nbFiles],ent->d_name);
                nbFiles++;
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
    char * l = "*c";
    char * l2 = malloc(sizeof(char) * 4096);
    strcpy(l2,l);
    char ** files = getPathSplit(l2);
    char * path = malloc(sizeof(char) * 4096);

    char ** files2 = getFiles(".", files[0], "ls");

    char ** res = malloc(sizeof(char *) * 4096);
    int i = 0;
    // while (files[i] != NULL) {
    //     printf("%s\n",files[i]);
    //     i++;
    // }
    execvp("ls",files2);

    return 0;
}



// int star (int argc, char ** argv) {
    // int fd = open(".", O_RDONLY);
    // //TODO: Separer chaque argument de argv dans un tableau de string apres le "/"

    // char ** cmd = malloc(sizeof(char *) * 4096);
    // for (int i = 0; i< 4096; i++) {
    //   cmd[i] = malloc(sizeof(char) * 4096);
    // }
    // struct string * path = string_new(PATH_MAX);
    // string_append(path, ".");
    // int j = oe(fd,1,path,cmd, argv);
    // cmd[0] = argv[0];

    // cmd[j] = NULL;
    // cmd = realloc(cmd,(j+1)*sizeof(char *));
    // for(int i = 0 ; i < j; i++) {
    //   printf("%s \n",cmd[i]);
    // }
    // execvp(cmd[0],cmd);
    // perror("exec");
    // return 0;
// }
