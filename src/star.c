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


int oe (int fd,int i,struct string * path,char ** cmd, char ** st) {
    DIR * dir = NULL;
    struct dirent * entry;
    dir = fdopendir(fd);
    struct string * s = string_new(PATH_MAX);
    if (strcmp(path -> data,".") !=  0) {
      string_append(s,path -> data);
      string_append(s,"/");
    }
    pile * p = pile_init();
    // int j = 0;
    // while (st[j] != NULL){
    //     printf("st[%d] = %s | ",j,st[j]);
    //     j++;}
    while ((entry = readdir(dir))) {
        if(strcmp(entry -> d_name,"..") == 0 || strcmp(entry -> d_name,".") == 0 || entry -> d_name[0] == '.') {
            continue;
        }
        // le fichier est un repertoire on le met dans la pile 
        if (entry -> d_type == DT_DIR) {
            struct string * new = string_new(PATH_MAX);
            string_append(new,path -> data);
            string_append(new,"/");
            string_append(new,entry-> d_name);
            if (isFormat(entry -> d_name, st[1]))
                empile(p,elem_init(new));
        }
        if (isFormat(entry -> d_name, st[1])) {
        // ajout dans le string le nom du fichier
            string_append(s,entry -> d_name);
        // ajout dans cmd le chemin du fichier
            strcpy(cmd[i++],s -> data);
        // enleve de s le dernier fichier ajoute
            string_truncate(s,strlen(entry -> d_name));
        }
    }

    // d = tete de la pile
    elem * d = depile(p);
    // remove from st the part before the first
    int res = 0;
    size_t len = strlen(st[1]);
    for (size_t i = 0; i < len; i++)
    {
        if (st[1][i] == '/') {
            res = i;
            break;
        }
    }
    st[1] = st[1] + res + 1;
    printf("st[1] = %s \n",st[1]);


    while (d) {
        // ouverture du repertoire en bout de pile
        int son_fd = open(d -> path -> data,O_RDONLY);
        i = oe(son_fd,i,d -> path,cmd, st);
        elem * f = d;
        d = depile(p);
        // printf("d -> data = %s\n", d -> path -> data);
        string_delete(f -> path);
        free(f);
    }

    free(p);
    string_delete(s);
    closedir(dir);
    return i;
}



int star (int argc, char ** argv) {
    int fd = open(".",O_RDONLY);
    char ** cmd = malloc(sizeof(char *) * 4096);
    for (int i = 0; i< 4096; i++) {
      cmd[i] = malloc(sizeof(char) * 4096);
    }
    struct string * path = string_new(PATH_MAX);
    string_append(path, ".");
    int j = oe(fd,1,path,cmd, argv);
    cmd[0] = argv[0];
    cmd[j] = NULL;
    cmd = realloc(cmd,(j+1)*sizeof(char *));
    // for(int i = 0 ; i < j; i++) {
    //   printf("%s \n",cmd[i]);
    // }
    execvp(cmd[0],cmd);
    perror("exec");
    return 0;
}